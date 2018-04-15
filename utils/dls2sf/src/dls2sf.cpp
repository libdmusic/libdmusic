#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <dmusic/Riff.h>
#include <dmusic/dls/DownloadableSound.h>
#include <dmusic/Forms.h>
#include <dmusic/Tracks.h>
#include <sf2cute.hpp>
#include <climits>
#include <cassert>
#include <cmath>
#include <map>
#include <set>
#include <exception>
#include <args.hxx>
#include "../../src/decode.h"

using namespace DirectMusic;
using namespace sf2cute;

template<typename T>
struct GuidObjectComparer {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs.getGuid() < rhs.getGuid();
    }
};

static Riff::Chunk loadChunk(std::string path) {
    std::ifstream inputStream(path, std::ios::binary | std::ios::ate);
    if (!inputStream.is_open()) {
        throw std::runtime_error("Couldn't open file " + path);
    }
    std::vector<std::uint8_t> buffer(inputStream.tellg());
    inputStream.seekg(0);
    inputStream.read((char*)buffer.data(), buffer.size());
    inputStream.close();
    Riff::Chunk ch(buffer.data());
    return ch;
}

static double dwordTimecentsToSeconds(std::int32_t tc) {
    return exp2((double)tc / (1200.0 * 65536.0));
}

static std::int16_t secondsToWordTimecents(double secs) {
    return (std::int16_t)(1200*log2(secs));
}

// FIXME: Only "envelope" articulators are supported
static void insertArticulator(const DLS::Articulator& articulator, std::vector<SFModulatorItem>& mods, std::vector<SFGeneratorItem>& gens) {
    for (const auto& connBlock : articulator.getConnectionBlocks()) {
        if (connBlock.usControl == (DLS::ArticulatorControl)0 &&
            connBlock.usSource == DLS::ArticulatorSource::None &&
            connBlock.usTransform == DLS::ArticulatorTransform::None) {
            SFGenerator gen;
            switch (connBlock.usDestination) {
            case DLS::ArticulatorDestination::EG1AttachTime:
                gen = SFGenerator::kAttackVolEnv; break;
            case DLS::ArticulatorDestination::EG1DecayTime:
                gen = SFGenerator::kDecayVolEnv; break;
            case DLS::ArticulatorDestination::EG1ReleaseTime:
                gen = SFGenerator::kReleaseVolEnv; break;
            case DLS::ArticulatorDestination::EG1SustainLevel:
                gen = SFGenerator::kSustainVolEnv; break;

            case DLS::ArticulatorDestination::EG2AttachTime:
                gen = SFGenerator::kAttackModEnv; break;
            case DLS::ArticulatorDestination::EG2DecayTime:
                gen = SFGenerator::kDecayModEnv; break;
            case DLS::ArticulatorDestination::EG2ReleaseTime:
                gen = SFGenerator::kReleaseModEnv; break;
            case DLS::ArticulatorDestination::EG2SustainLevel:
                gen = SFGenerator::kSustainModEnv; break;
            }
            double secs = dwordTimecentsToSeconds(connBlock.lScale);
            std::int16_t newValue = secondsToWordTimecents(secs);
            gens.push_back(SFGeneratorItem(gen, GenAmountType(newValue)));
        }
    }
}

static std::map<GUID, DLS::DownloadableSound> loaded_sounds;
static std::map<GUID, std::set<DLS::DownloadableSound, GuidObjectComparer<DLS::DownloadableSound>>> loaded_bands;

static void convertSounds(std::vector<DLS::DownloadableSound>& sounds, std::ostream& ofs) {
    SoundFont sf2;

    // Each sample may potentially be referenced with a different basenote,
    // so we keep them here and copy them as needed.
    std::vector<std::vector<SFSample>> samples;
    for (int i = 0; i < sounds.size(); i++) {
        auto& sound = sounds[i];
        samples.push_back(std::vector<SFSample>());
        auto& wavePool = sound.getWavePool();

        while (!wavePool.empty()) {
            auto& wav = wavePool[0];
            std::string name = wav.getInfo().getName();
            auto fmt = wav.getWaveformat();

            // DLS lev. 1 only supports PCM16 samples, but
            // we need to load encoded samples as well, so
            // we make libsndfile take care of that
            std::vector<std::int16_t> audioData = decode(wav);

            if (audioData.empty()) {
                throw std::runtime_error("Invalid sample format for " + name);
            }

            auto wavsmpl = wav.getWavesample();
            std::uint32_t startLoop, endLoop;
            std::uint32_t midiNote, fineTune;

            if (wavsmpl.cSampleLoops == 0) {
                // In case the sample doesn't have loops,
                // we just loop the last two samples.
                startLoop = audioData.size() - 2;
                endLoop = audioData.size() - 1;
            } else {
                auto waveLoop = wav.getWavesampleLoops()[0];
                startLoop = waveLoop.ulLoopStart;
                endLoop = waveLoop.ulLoopStart + waveLoop.ulLoopLength;
            }
            wavePool.erase(wavePool.begin());

            midiNote = wavsmpl.usUnityNote;
            fineTune = wavsmpl.sFineTune;

            samples[i].push_back(SFSample(name,
                audioData,
                startLoop, endLoop,
                fmt.dwSamplesPerSec,
                midiNote,
                fineTune)); // FIXME: we may need to do some work on this
        }
    }

    std::vector<std::shared_ptr<SFInstrument>> instruments;
    std::vector<std::shared_ptr<SFPreset>> presets;

    for (int i = 0; i < sounds.size(); i++) {
        for (const DLS::Instrument& instr : sounds[i].getInstruments()) {
            std::vector<SFInstrumentZone> zones;
            for (DLS::Region reg : instr.getRegions()) {
                auto hdr = reg.getRegionHeader();
                auto wavelink = reg.getWaveLink();
                auto wavesample = reg.getWavesample();
                std::vector<SFGeneratorItem> genItems;
                std::vector<SFModulatorItem> modItems;
                std::shared_ptr<SFSample> sample;
                std::uint16_t keyrangeLow, keyrangeHigh, velrangeLow, velrangeHigh;
                genItems.push_back(SFGeneratorItem(SFGenerator::kAttackVolEnv, GenAmountType(secondsToWordTimecents(0.1))));

                for (const auto& art : instr.getArticulators()) {
                    insertArticulator(art, modItems, genItems);
                }

                assert(hdr.RangeKey.usHigh >= hdr.RangeKey.usLow);

                keyrangeLow = hdr.RangeKey.usLow;
                keyrangeHigh = hdr.RangeKey.usHigh;

                if (hdr.RangeVelocity.usHigh - hdr.RangeVelocity.usLow <= 0) {
                    velrangeLow = 0;
                    velrangeHigh = 127;
                } else {
                    velrangeLow = hdr.RangeVelocity.usLow;
                    velrangeHigh = hdr.RangeVelocity.usHigh;
                }

                genItems.push_back(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(keyrangeLow, keyrangeHigh)));
                genItems.push_back(SFGeneratorItem(SFGenerator::kVelRange, RangesType(velrangeLow, velrangeHigh)));
                if (wavesample.cSampleLoops == 0) {
                    sample = sf2.NewSample(samples[i][wavelink.ulTableIndex]);
                    sample->set_start_loop(sample->data().size() - 2);
                    sample->set_end_loop(sample->data().size() - 1);
                    genItems.push_back(SFGeneratorItem(SFGenerator::kSampleModes, std::uint16_t(SampleMode::kNoLoop)));
                } else {
                    auto loop = reg.getWavesampleLoops()[0];
                    sample = sf2.NewSample(samples[i][wavelink.ulTableIndex]);
                    sample->set_start_loop(loop.ulLoopStart);
                    sample->set_end_loop(loop.ulLoopStart + loop.ulLoopLength);
                    genItems.push_back(SFGeneratorItem(SFGenerator::kSampleModes, std::uint16_t(SampleMode::kLoopContinuously)));
                }
                sample->set_original_key(wavesample.usUnityNote);
                sample->set_correction(wavesample.sFineTune);
                SFInstrumentZone zone(sample, genItems, modItems);
                zones.push_back(zone);
            }
            auto instrument = sf2.NewInstrument(instr.getInfo().getName(), zones);
            instruments.push_back(instrument);
            presets.push_back(sf2.NewPreset(instrument->name(), instr.getMidiProgram(), 0,
                std::vector<SFPresetZone>{
                SFPresetZone(instrument)
            }));
        }
    }

    sf2.Write(ofs);
}

static void convertBand(const BandForm& band, const std::string& inputDir) {
    for (const auto& instr : band.getInstruments()) {
        if (instr.getReference() == nullptr) continue;
        if (loaded_sounds.find(instr.getReference()->getGuid()) == loaded_sounds.end()) {
            std::cout << "Loading " << instr.getReference()->getFile() << " ... ";
            auto sound = DLS::DownloadableSound(loadChunk(inputDir + "/" + instr.getReference()->getFile()));
            loaded_bands[band.getGuid()].insert(sound);
            loaded_sounds[sound.getGuid()] = sound;
            std::cout << "Done." << std::endl;
        } else {
            loaded_bands[band.getGuid()].insert(loaded_sounds.at(instr.getReference()->getGuid()));
        }
    }
}

static void convertStyles(const std::vector<StyleForm>& styles, const std::string& inputDir) {
    std::set<BandForm, GuidObjectComparer<BandForm>> bands;
    for (const auto& style : styles) {
        for (const auto& band : style.getBands()) {
            convertBand(band, inputDir);
        }
    }
}

static void convertSegments(const std::vector<SegmentForm>& segments, const std::string& inputDir) {
    std::set<BandForm, GuidObjectComparer<BandForm>> bands;
    for (const auto& segment : segments) {
        for (const auto& track : segment.getTracks()) {
            const auto& header = track.getHeader();
            std::string ckid = std::string(header.ckid),
                fccType = std::string(header.fccType);

            ckid.resize(4);
            fccType.resize(4);

            if (*header.ckid == 0 && fccType == "DMBT") {
                auto bandTrack = std::static_pointer_cast<BandTrack>(track.getData());
                for (const auto& band : bandTrack->getBands()) {
                    DMUS_IO_BAND_ITEM_HEADER2 header = band.first;
                    BandForm bandForm = band.second;

                    convertBand(bandForm, inputDir);
                }
            }
        }
    }
}

int main(int argc, char **argv) {
    args::ArgumentParser parser("dls2sf converts DLS files and DirectMusic bands to SoundFont2 files");
    args::HelpFlag help(parser, "help", "Display this help menu", { 'h', "help" });
    args::Flag segment(parser, "segment", "Treats the input files as segments, and converts every band found", { 's', "segment" });
    args::ValueFlag<std::string> output(parser, "output", "Output file/path", { 'o', "output" });
    args::ValueFlag<std::string> workingPath(parser, "input directory", "Input path where sounds are located", { 'i', "input" });
    args::PositionalList<std::string> input(parser, "input files", "Input files to parse");

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << parser;
        return 0;
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    } catch (args::ValidationError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    if (!input) {
        std::cerr << "dls2sf: no input specified." << std::endl;
        return 1;
    }

    if (segment) {
        std::string outputDir = output ? args::get(output) : ".";
        std::string wPath = workingPath ? args::get(workingPath) : ".";
        std::vector<SegmentForm> segments;
        std::vector<StyleForm> styles;
        for (const auto& file : args::get(input)) {
            std::cout << "Loading " << file << " ... ";
            Riff::Chunk chnk = loadChunk(file);
            if (chnk.getListId() == "DMSG") {
                segments.push_back(SegmentForm(loadChunk(file)));
            } else if (chnk.getListId() == "DMST") {
                styles.push_back(StyleForm(loadChunk(file)));
            } else {
                std::cerr << "Invalid chunk type: " << chnk.getId() << " " << chnk.getListId() << std::endl;
                return 1;
            }
            std::cout << "Done." << std::endl;
        }
        convertSegments(segments, wPath);
        convertStyles(styles, wPath);

        for (const auto& band : loaded_bands) {
            std::vector<DLS::DownloadableSound> sounds(std::begin(band.second), std::end(band.second));
            std::string outputFile = outputDir + "/" + band.first.toString() + ".sf2";
            std::cout << "Writing " << outputFile << " ... ";
            std::ofstream ofs(outputFile, std::ios::binary);
            convertSounds(sounds, ofs);
            ofs.close();
            std::cout << "Done.\n";
        }
        std::cout << "All done." << std::endl;
    } else {
        std::vector<DLS::DownloadableSound> sounds;

        for (const auto& sound : args::get(input)) {
            std::cout << "Loading " << sound << " ... ";
            sounds.push_back(DLS::DownloadableSound(loadChunk(sound)));
            std::cout << "Done." << std::endl;
        }

        if (output) {
            std::ofstream ofs(args::get(output), std::ios::binary);
            convertSounds(sounds, ofs);
            ofs.close();
        } else {
            convertSounds(sounds, std::cout);
        }
    }
    return 0;
}
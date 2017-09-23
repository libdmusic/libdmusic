#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <dmusic/Riff.h>
#include <dmusic/dls/DownloadableSound.h>
#include <sf2cute.hpp>
#include <climits>
#include "decode.h"

using namespace DirectMusic;
using namespace sf2cute;

static Riff::Chunk loadChunk(std::string path) {
    std::ifstream inputStream(path, std::ios::binary | std::ios::ate);
    if (!inputStream.is_open()) {
        throw "Couldn't open file";
    }
    std::vector<std::uint8_t> buffer(inputStream.tellg());
    inputStream.seekg(0);
    inputStream.read((char*)buffer.data(), buffer.size());
    inputStream.close();
    Riff::Chunk ch(buffer.data());
    return ch;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Usage: dls2sf <inputfile> <outputfile>" << std::endl;
        return 1;
    }

    std::string inputFile = std::string(argv[1]);
    std::string outputFile = std::string(argv[2]);

    std::cout << "Parsing input file... ";
    Riff::Chunk chunk = loadChunk(inputFile);
    std::cout << "Done.\nLoading DLS structure... ";
    DLS::DownloadableSound dls(chunk);
    std::cout << "Done.\n";
    SoundFont sf2;
    const Riff::Info& info = dls.getInfo();
    sf2.set_bank_name(info.getName());
    sf2.set_comment(info.getComments());
    sf2.set_copyright(info.getCopyright());
    sf2.set_engineers(info.getEngineer());
    sf2.set_creation_date(info.getCreationDate());

    std::cout << "Loading samples... ";
    // Each sample may potentially be referenced with a different basenote,
    // so we keep them here and copy them as needed.
    std::vector<SFSample> samples;

    for (const DLS::Wave& wav : dls.getWavePool()) {
        std::string name = wav.getInfo().getName();
        auto fmt = wav.getWaveformat();

        // DLS lev. 1 only supports PCM16 samples, but
        // we need to load encoded samples as well, so
        // we make libsndfile take care of that
        std::vector<std::int16_t> audioData = decode(wav);

        if (audioData.empty()) {
            std::cerr << "Invalid sample format for " << name << std::endl;
            return 1;
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

        midiNote = wavsmpl.usUnityNote;
        fineTune = wavsmpl.sFineTune;

        samples.push_back(SFSample(name,
            audioData,
            startLoop, endLoop,
            fmt.dwSamplesPerSec,
            midiNote,
            fineTune)); // FIXME: we may need to do some work on this
    }
    std::cout << "Done: " << samples.size() << " samples loaded.\n";

    std::vector<std::shared_ptr<SFInstrument>> instruments;
    std::vector<std::shared_ptr<SFPreset>> presets;

    for (const DLS::Instrument& instr : dls.getInstruments()) {
        std::cout << "Converting instrument '" << instr.getInfo().getName() << "'... ";
        std::vector<SFInstrumentZone> zones;
        for (DLS::Region reg : instr.getRegions()) {
            auto hdr = reg.getRegionHeader();
            auto wavelink = reg.getWaveLink();
            auto wavesample = reg.getWavesample();
            std::vector<SFGeneratorItem> genItems;
            std::vector<SFModulatorItem> modItems;
            std::shared_ptr<SFSample> sample;
            std::uint16_t keyrangeLow, keyrangeHigh, velrangeLow, velrangeHigh;

            if (hdr.RangeKey.usHigh - hdr.RangeKey.usLow <= 0) {
                // Sometimes, the range is just 0-0, I assume that means
                // it spans the entire keyboard
                keyrangeLow = 0;
                keyrangeHigh = 127;
            } else {
                keyrangeLow = hdr.RangeKey.usLow;
                keyrangeHigh = hdr.RangeKey.usHigh;
            }

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
                sample = sf2.NewSample(samples[wavelink.ulTableIndex]);
                sample->set_start_loop(sample->data().size() - 2);
                sample->set_end_loop(sample->data().size() - 1);
                genItems.push_back(SFGeneratorItem(SFGenerator::kSampleModes, std::uint16_t(SampleMode::kNoLoop)));
            } else {
                auto loop = reg.getWavesampleLoops()[0];
                sample = sf2.NewSample(samples[wavelink.ulTableIndex]);
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
        presets.push_back(sf2.NewPreset(instrument->name(), 0, 0,
            std::vector<SFPresetZone>{
            SFPresetZone(instrument)
        }));
        std::cout << "Done: " << zones.size() << " zones loaded.\n";
    }
    std::cout << instruments.size() << " instruments converted.\n";

    std::cout << "Writing output file... ";
    std::ofstream ofs(outputFile, std::ios::binary);
    sf2.Write(ofs);
    ofs.close();
    std::cout << "Done.\n";
    return 0;
}
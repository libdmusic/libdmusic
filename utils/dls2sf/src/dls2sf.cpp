#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <dmusic/Riff.h>
#include <dmusic/dls/DownloadableSound.h>
#include <sf2cute.hpp>
#include <climits>

using namespace DirectMusic;
using namespace sf2cute;

template <typename T>
T swap_endian(T u) {
    static_assert (CHAR_BIT == 8, "CHAR_BIT != 8");

    union {
        T u;
        unsigned char u8[sizeof(T)];
    } source, dest;

    source.u = u;

    for (size_t k = 0; k < sizeof(T); k++)
        dest.u8[k] = source.u8[sizeof(T) - k - 1];

    return dest.u;
}

std::vector<std::int16_t> convert(std::vector<std::uint8_t> in) {
    std::vector<std::int16_t> vec(in.size() / 2);
    std::int16_t *buf = (std::int16_t*)in.data();

    for (int i = 0; i < vec.size(); i++) {
        vec[i] = /*swap_endian*/(buf[i]);
    }
    return vec;
}

Riff::Chunk loadChunk(std::string path) {
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
        std::cerr << "Usage: dls2sf [inputfile.dls] [outputfile.sf2]" << std::endl;
        return 1;
    }
    std::string inputFile = std::string(argv[1]);
    std::string outputFile = std::string(argv[2]);

    std::cout << "Parsing input file... ";
    Riff::Chunk chunk = loadChunk(inputFile);
    std::cout << "Done.\n";
    std::cout << "Loading DLS structure... ";
    DLS::DownloadableSound dls(chunk);
    std::cout << "Done.\n";
    SoundFont sf2;
    Riff::Info& info = dls.getInfo();
    sf2.set_bank_name(info.getName());
    sf2.set_comment(info.getComments());
    sf2.set_copyright(info.getCopyright());
    sf2.set_engineers(info.getEngineer());
    sf2.set_creation_date(info.getCreationDate());
    
    std::cout << "Loading samples... ";
    std::vector<std::shared_ptr<SFSample>> samples;
    for (const DLS::Wave& wav : dls.getWavePool()) {
        std::string name = wav.getInfo().getName();
        auto fmt = wav.getWaveformat();
        auto wavsmpl = wav.getWavesample();
        auto data = wav.getWavedata();
        std::uint32_t startLoop, endLoop;

        if (wavsmpl.cSampleLoops == 0) {
            startLoop = data.size() - 1;
            endLoop = data.size();
        } else {
            auto waveLoop = wav.getWavesampleLoops()[0];
            startLoop = waveLoop.ulLoopStart;
            endLoop = waveLoop.ulLoopLength;
        }

        samples.push_back(sf2.NewSample(name,
            convert(data),
            startLoop, endLoop,
            fmt.dwSamplesPerSec * 2,
            wavsmpl.usUnityNote,
            wavsmpl.sFineTune));
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
            genItems.push_back(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(hdr.RangeKey.usLow, hdr.RangeKey.usHigh)));
            genItems.push_back(SFGeneratorItem(SFGenerator::kVelRange, RangesType(hdr.RangeVelocity.usLow, hdr.RangeVelocity.usHigh)));
            if (wavesample.cSampleLoops == 0) {
                genItems.push_back(SFGeneratorItem(SFGenerator::kSampleModes, std::uint16_t(SampleMode::kNoLoop)));
            } else {
                genItems.push_back(SFGeneratorItem(SFGenerator::kSampleModes, std::uint16_t(SampleMode::kLoopContinuously)));
            }
            SFInstrumentZone zone(samples[wavelink.ulTableIndex], genItems, modItems);
            zones.push_back(zone);
        }
        auto instrument = sf2.NewInstrument(instr.getInfo().getName(), zones);
        instruments.push_back(instrument);
        presets.push_back(sf2.NewPreset(instrument->name(), 0, 0,
            std::vector<SFPresetZone>{
            SFPresetZone(instrument)
        }));
        std::cout << "Done.\n";
    }

    std::ofstream ofs(outputFile, std::ios::binary);
    sf2.Write(ofs);
    ofs.close();
    return 0;
}
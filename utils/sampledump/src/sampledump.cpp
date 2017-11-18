#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <exception>
#include <dmusic/Riff.h>
#include <dmusic/dls/DownloadableSound.h>

using namespace DirectMusic;

static Riff::Chunk loadChunk(std::string path) {
    std::ifstream inputStream(path, std::ios::binary | std::ios::ate);
    if (!inputStream.is_open()) {
        throw std::runtime_error("Couldn't open file");
    }
    std::vector<std::uint8_t> buffer(inputStream.tellg());
    inputStream.seekg(0);
    inputStream.read((char*)buffer.data(), buffer.size());
    inputStream.close();
    Riff::Chunk ch(buffer.data());
    return ch;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: sampledump <inputfile.dls>" << std::endl;
        return 1;
    }
    std::string inputFile = std::string(argv[1]);

    std::cout << "Parsing input file... ";
    Riff::Chunk chunk = loadChunk(inputFile);

    std::cout << "Done.\nLoading DLS structure... ";
    DLS::DownloadableSound dls(chunk);

    std::cout << "Done.\nDumping samples... ";
    for (const DLS::Wave& wav : dls.getWavePool()) {
        std::string name = wav.getInfo().getName();
        std::ofstream ofs(name + ".wav", std::ios::binary);
        wav.writeToStream(ofs);
        ofs.close();
    }
    std::cout << "Done: " << dls.getWavePool().size() << " samples dumped.\n";
    return 0;
}
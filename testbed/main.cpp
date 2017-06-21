#include <iostream>
#include <fstream>
#include <dmusic/Riff.h>

using namespace DirectMusic::Riff;

void printChunk(Chunk c, int offset) {
    for (int i = 0; i < offset; i++) std::cout << " ";
    std::cout << c.getId();
    if (!c.getSubchunks().empty()) {
        std::cout << " - " << c.getListId() << "\n";
        for (Chunk ch : c.getSubchunks()) {
            printChunk(ch, offset + 2);
        }
    } else {
        std::cout << "\n";
    }
}

int main(int argc, char **argv) {
    if(argc < 2) {
        std::cerr << "Usage: testbed <filename>\n";
        return 1;
    }
    std::string inputFile(argv[1]);
    std::ifstream stream(inputFile, std::ios::in | std::ios::binary | std::ios::ate);
    int offset = 0;
    if (!stream.is_open()) {
        std::cerr << "Couldn't open file " << inputFile << "\n";
        return 1;
    }
    int size = stream.tellg();
    std::vector<std::uint8_t> buffer;
    buffer.resize(size);
    char *data = (char*)buffer.data();
    stream.seekg(0, std::ios::beg);
    stream.read(data, size);
    stream.close();
    Chunk c(buffer, 0);
    printChunk(c, 0);
    return 0;
}

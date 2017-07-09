#include <fstream>
#include <dmusic/Loader.h>

using namespace DirectMusic;

std::vector<std::uint8_t> Loader::loadFile(const std::string& name) const {
    std::ifstream inputStream(name, std::ios::binary | std::ios::ate);
    if (!inputStream.is_open()) {
        return std::vector<std::uint8_t>();
    }
    std::vector<std::uint8_t> buffer(inputStream.tellg());
    inputStream.seekg(0);
    inputStream.read((char*)buffer.data(), buffer.size());
    inputStream.close();
    return buffer;
}

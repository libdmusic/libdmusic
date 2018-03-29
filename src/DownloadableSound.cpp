#include <fstream>
#include <vector>
#include <exception>
#include <dmusic/dls/DownloadableSound.h>
#include <dmusic/Exceptions.h>
#include <dmusic/dls/Instrument.h>

using namespace DirectMusic;
using namespace DirectMusic::Riff;
using namespace DirectMusic::DLS;

void DownloadableSound::loadChunk(const Chunk& c) {
    if (c.getId() != "RIFF" || c.getListId() != "DLS ")
        throw DirectMusic::InvalidChunkException("RIFF DLS", c.getId() + " " + c.getListId());

    for (const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        if (id == "vers") {
            m_version = littleEndianRead<std::uint64_t>(subchunk.getData().data());
        } else if (id == "dlid") {
            m_dlsid = GUID(subchunk.getData().data());
        } else if (id == "colh") {
            //We dynamically build the vector, we don't need the size for now
        } else if (id == "ptbl") {
            PoolTable ptable(subchunk.getData().data());
            const std::uint8_t *data = subchunk.getData().data() + ptable.cbSize;
            for (int i = 0; i < ptable.cCues; i++) {
                m_poolOffsets.push_back(littleEndianRead<std::uint32_t>(data));
                data += 4;
            }
        } else if (id == "LIST") {
            std::string listId = subchunk.getListId();
            if (listId == "lins") {
                for (Chunk ins : subchunk.getSubchunks()) {
                    m_instruments.push_back(Instrument(ins));
                }
            } else if (listId == "wvpl") {
                for (Chunk wav : subchunk.getSubchunks()) {
                    m_wavePool.push_back(Wave(wav));
                }
            } else if (listId == "INFO") {
                m_info = Info(subchunk);
            }
        }
    }
}

DownloadableSound::DownloadableSound(const Chunk& c) {
    loadChunk(c);
}

DownloadableSound::DownloadableSound(const std::string& path) {
    std::ifstream inputStream(path, std::ios::binary | std::ios::ate);
    if (!inputStream.is_open()) {
        throw std::runtime_error("Cannot open file " + path);
    }
    std::vector<std::uint8_t> buffer(inputStream.tellg());
    inputStream.seekg(0);
    inputStream.read((char*)buffer.data(), buffer.size());
    inputStream.close();

    loadChunk(Chunk(buffer.data()));
}

bool DownloadableSound::operator==(const DownloadableSound& a) const {
    return a.m_dlsid == m_dlsid &&
        a.m_info == m_info &&
        a.m_instruments.size() == m_instruments.size();
}
#include <dmusic/dls/DownloadableSound.h>
#include <dmusic/Exceptions.h>
#include <dmusic/dls/Instrument.h>

using namespace DirectMusic;
using namespace DirectMusic::Riff;
using namespace DirectMusic::DLS;

DownloadableSound::DownloadableSound(Chunk& c) {
    if (c.getId() != "RIFF" || c.getListId() != "DLS ")
        throw DirectMusic::InvalidChunkException("RIFF DLS", c.getId() + " " + c.getListId());

    for (Chunk subchunk : c.getSubchunks()) {
        std::string id = subchunk.getId();
        if (id == "vers") {
            m_version = littleEndianRead<std::uint64_t>(subchunk.getData().data());
        } else if (id == "dlid") {
            m_dlsid = *((GUID*)subchunk.getData().data()); // FIXME: This should be read according to endnianness,
                                                           // but we don't really use this field anyway...
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
                for (Chunk ins: subchunk.getSubchunks()) {
                    m_instruments.push_back(Instrument(ins));
                }
            } else if (listId == "wvpl") {
                for (Chunk wav: subchunk.getSubchunks()) {
                    m_wavePool.push_back(Wave(wav));
                }
            } else if (listId == "INFO") {
                m_info = Info(subchunk);
            }
        }
    }
}

const std::vector<Instrument>& DownloadableSound::getInstruments() const {
    return m_instruments;
}

const std::vector<std::uint32_t>& DownloadableSound::getPoolOffsets() const {
    return m_poolOffsets;
}

const std::vector<Wave>& DownloadableSound::getWavePool() const {
    return m_wavePool;
}

Info& DownloadableSound::getInfo() {
    return m_info;
}

const GUID& DownloadableSound::getGuid() const {
    return m_dlsid;
}
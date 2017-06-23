#include <dmusic/dls/DownloadableSound.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic;
using namespace DirectMusic::Riff;
using namespace DirectMusic::DLS;

Wave::Wave(Chunk& c) {
    if (c.getId().compare("LIST") != 0 || c.getListId().compare("wave") != 0)
        throw DirectMusic::InvalidChunkException("LIST wave", c.getId() + " " + c.getListId());

    for (Chunk subchunk : c.getSubchunks()) {
        std::string id = subchunk.getId();
        if (!id.compare("dlid")) {
            m_dlsid = *((GUID*)subchunk.getData().data());
        } else if (!id.compare("fmt ")) {
            m_fmt = *((WaveFormat*)subchunk.getData().data());
        } else if (!id.compare("wsmp")) {
            m_wavesample = *((Wavesample*)subchunk.getData().data());
            if (m_wavesample.cSampleLoops > 0) {
                WavesampleLoop *loops = (WavesampleLoop*)(subchunk.getData().data() + m_wavesample.cbSize);
                for (int i = 0; i < m_wavesample.cSampleLoops; i++) {
                    m_loops.push_back(loops[i]);
                }
            }
        } else if (!id.compare("LIST") && !subchunk.getListId().compare("INFO")) {
            m_info = Info(subchunk);
        } else if (!id.compare("data")) {
            m_wavedata = subchunk.getData();
        }
    }
}

const GUID& Wave::getGuid() const {
    return m_dlsid;
}

const DirectMusic::Riff::Info& Wave::getInfo() const {
    return m_info;
}

const WaveFormat& Wave::getWaveformat() const {
    return m_fmt;
}

const Wavesample& Wave::getWavesample() const {
    return m_wavesample;
}

const std::vector<uint8_t>& Wave::getWavedata() const {
    return m_wavedata;
}

const std::vector<WavesampleLoop>& Wave::getWavesampleLoops() const {
    return m_loops;
}
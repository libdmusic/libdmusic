#include <dmusic/dls/DownloadableSound.h>
#include <dmusic/Exceptions.h>
#include <iostream>
#include <fstream>

using namespace DirectMusic;
using namespace DirectMusic::Riff;
using namespace DirectMusic::DLS;

Wave::Wave(Chunk& c) : m_containsSampler(false) {
    if (c.getId() != "LIST" || c.getListId() != "wave")
        throw DirectMusic::InvalidChunkException("LIST wave", c.getId() + " " + c.getListId());

    for (Chunk subchunk : c.getSubchunks()) {
        std::string id = subchunk.getId();
        if (id == "dlid") {
            m_dlsid = *((GUID*)subchunk.getData().data());
        } else if (id == "fmt ") {
            m_fmt = *((WaveFormat*)subchunk.getData().data());
        } else if (id == "wsmp") {
            m_wavesample = *((Wavesample*)subchunk.getData().data());
            if (m_wavesample.cSampleLoops > 0) {
                WavesampleLoop *loops = (WavesampleLoop*)(subchunk.getData().data() + m_wavesample.cbSize);
                for (int i = 0; i < m_wavesample.cSampleLoops; i++) {
                    m_loops.push_back(loops[i]);
                }
            }
        } else if(id == "smpl") {
            m_sampler = *((Sampler*)subchunk.getData().data());
            if (m_sampler.numSampleLoops > 0) {
                SamplerLoop *loops = (SamplerLoop*)(subchunk.getData().data() + sizeof(Sampler));
                for (int i = 0; i < m_sampler.numSampleLoops; i++) {
                    m_samplerLoops.push_back(loops[i]);
                }
            }
            m_containsSampler = true;
        } else if (id == "LIST" && subchunk.getListId() == "INFO") {
            m_info = Info(subchunk);
        } else if (id == "data") {
            m_wavedata = subchunk.getData();
        }
    }
}

void Wave::writeToStream(std::ostream& stream) const {
    int dataSize = m_wavedata.size();
    int totalSize = sizeof(WaveFormat) + 20 + dataSize + (sizeof(WaveFormat) % 2) + (m_wavedata.size() % 2);
    int fmtSize = sizeof(WaveFormat);
    stream.write("RIFF", 4);
    stream.write((const char*)(&totalSize), 4);
    stream.write("WAVE", 4);
    stream.write("fmt ", 4);
    stream.write((const char*)&fmtSize, 4);
    stream.write((const char*)&m_fmt, sizeof(WaveFormat));
    if (sizeof(WaveFormat) % 2 == 1) stream.write("\0", 1);
    stream.write("data", 4);
    stream.write((const char*)&dataSize, 4);
    stream.write((const char*)m_wavedata.data(), m_wavedata.size());
    if (m_wavedata.size() % 2 == 1) stream.write("\0", 1);
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

const Sampler& Wave::getSampler() const {
    return m_sampler;
}

const std::vector<uint8_t>& Wave::getWavedata() const {
    return m_wavedata;
}

const std::vector<WavesampleLoop>& Wave::getWavesampleLoops() const {
    return m_loops;
}

const std::vector<SamplerLoop>& Wave::getSamplerLoops() const {
    return m_samplerLoops;
}

const bool Wave::containsSampler() const {
    return m_containsSampler;
}
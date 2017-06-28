#include <dmusic/dls/DownloadableSound.h>
#include <dmusic/Exceptions.h>
#include <iostream>
#include <fstream>

using namespace DirectMusic;
using namespace DirectMusic::Riff;
using namespace DirectMusic::DLS;

Wave::Wave(Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "wave")
        throw DirectMusic::InvalidChunkException("LIST wave", c.getId() + " " + c.getListId());

    for (Chunk subchunk : c.getSubchunks()) {
        std::string id = subchunk.getId();
        if (id == "dlid") {
            m_dlsid = *((GUID*)subchunk.getData().data());
        } else if (id == "fmt ") {
            m_fmtex = *((WaveFormatEx*)subchunk.getData().data());
            if (m_fmtex.cbSize > 0 && subchunk.getData().size() > sizeof(WaveFormat)) {
                const std::uint8_t *extraData = subchunk.getData().data() + sizeof(WaveFormatEx);
                m_extraFmtData = std::vector<std::uint8_t>(extraData, extraData + m_fmtex.cbSize);
            } else {
                m_fmtex.cbSize = 0;
            }
        } else if (id == "wsmp") {
            m_wavesample = *((Wavesample*)subchunk.getData().data());
            if (m_wavesample.cSampleLoops > 0) {
                WavesampleLoop *loops = (WavesampleLoop*)(subchunk.getData().data() + m_wavesample.cbSize);
                for (int i = 0; i < m_wavesample.cSampleLoops; i++) {
                    m_loops.push_back(loops[i]);
                }
            }
        } else if (id == "LIST" && subchunk.getListId() == "INFO") {
            m_info = Info(subchunk);
        } else if (id == "data") {
            m_wavedata = subchunk.getData();
        }
    }
}

std::vector<std::uint8_t> Wave::getWaveFile() const {
    int dataSize = m_wavedata.size();
    int totalSize = sizeof(WaveFormatEx) + m_extraFmtData.size() + dataSize + ((sizeof(WaveFormat) + m_extraFmtData.size()) % 2) + (m_wavedata.size() % 2) + 30;
    int fmtSize = sizeof(WaveFormatEx) + m_extraFmtData.size();
    int factSize = 4;
    int factData = 1;
    int zero = 0;

    std::vector<std::uint8_t> output(totalSize + 8);
    std::uint8_t *data = output.data();

    memcpy(data, "RIFF", 4);
    data += 4;

    memcpy(data, (const char*)(&totalSize), 4);
    data += 4;

    memcpy(data, "WAVE", 4);
    data += 4;

    memcpy(data, "fmt ", 4);
    data += 4;
    memcpy(data, (const char*)(&fmtSize), 4);
    data += 4;
    memcpy(data, (const char*)(&m_fmtex), sizeof(WaveFormatEx));
    data += sizeof(WaveFormatEx);
    if (m_extraFmtData.size() > 0) {
        memcpy(data, m_extraFmtData.data(), m_extraFmtData.size());
        data += m_extraFmtData.size();
    }
    data += (sizeof(WaveFormatEx) + m_extraFmtData.size()) % 2;

    memcpy(data, "data", 4);
    data += 4;

    memcpy(data, (const char*)(&dataSize), 4);
    data += 4;

    memcpy(data, (const char*)(m_wavedata.data()), m_wavedata.size());
    data += m_wavedata.size();

    data += m_wavedata.size() % 2;

    memcpy(data, "fact", 4);
    data += 4;
    memcpy(data, (const char*)(&factSize), 4);
    data += 4;
    memcpy(data, (const char*)(&factData), 4);
    data += 4;

    return output;
}

void Wave::writeToStream(std::ostream& stream) const {
    auto wavefile = getWaveFile();
    stream.write((const char*)wavefile.data(), wavefile.size());
}

const GUID& Wave::getGuid() const {
    return m_dlsid;
}

const DirectMusic::Riff::Info& Wave::getInfo() const {
    return m_info;
}

const WaveFormatEx& Wave::getWaveformat() const {
    return m_fmtex;
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
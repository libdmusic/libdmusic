#include <dmusic/dls/DownloadableSound.h>
#include <dmusic/Exceptions.h>
#include <iostream>

using namespace DirectMusic;
using namespace DirectMusic::Riff;
using namespace DirectMusic::DLS;

Wave::Wave(const Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "wave")
        throw DirectMusic::InvalidChunkException("LIST wave", c.getId() + " " + c.getListId());

    for (const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        if (id == "dlid") {
            m_dlsid = GUID(subchunk.getData().data());
        } else if (id == "fmt ") {
            m_fmtex = WaveFormatEx(subchunk.getData().data());
            if (m_fmtex.cbSize > 0 && subchunk.getData().size() > sizeof(WaveFormatEx)) {
                const std::uint8_t *extraData = subchunk.getData().data() + sizeof(WaveFormatEx);
                m_extraFmtData = std::vector<std::uint8_t>(extraData, extraData + m_fmtex.cbSize);
            } else {
                m_fmtex.cbSize = 0;
            }
        } else if (id == "wsmp") {
            m_wavesample = Wavesample(subchunk.getData().data());
            if (m_wavesample.cSampleLoops > 0) {
                const std::uint8_t *data = subchunk.getData().data() + m_wavesample.cbSize;
                for (int i = 0; i < m_wavesample.cSampleLoops; i++) {
                    m_loops.push_back(WavesampleLoop(data));
                    data += sizeof(WavesampleLoop);
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

    int totalSize = 
        4 + // RIFF
        4 + // Total Size
        4 + // WAVE
        4 + // FMT
        4 + // sizeof(WaveFormatEx) + m_extraFmtData.size()
        sizeof(WaveFormatEx) +
        m_extraFmtData.size() +
        m_extraFmtData.size() % 2 +
        4 + // data
        4 + // m_wavedata.size()
        m_wavedata.size() +
        m_wavedata.size() % 2 +
        4 + // fact
        4 +
        4;

    std::vector<std::uint8_t> output(totalSize);
    std::uint8_t *data = output.data();

    memcpy(data, "RIFF", 4); data += 4;

    littleEndianWrite<std::uint32_t>(totalSize - 8, data); data += 4;

    memcpy(data, "WAVE", 4); data += 4;

    memcpy(data, "fmt ", 4); data += 4;

    littleEndianWrite<std::uint32_t>(sizeof(WaveFormatEx) + m_extraFmtData.size(), data); data += 4;
    littleEndianWrite<std::uint16_t>((std::uint16_t)m_fmtex.wFormatTag, data); data += 2;
    littleEndianWrite<std::uint16_t>(m_fmtex.wChannels, data); data += 2;
    littleEndianWrite<std::uint32_t>(m_fmtex.dwSamplesPerSec, data); data += 4;
    littleEndianWrite<std::uint32_t>(m_fmtex.dwAvgBytesPerSec, data); data += 4;
    littleEndianWrite<std::uint16_t>(m_fmtex.wBlockAlign, data); data += 2;
    littleEndianWrite<std::uint16_t>(m_fmtex.wBitsPerSample, data); data += 2;
    littleEndianWrite<std::uint16_t>(m_fmtex.cbSize, data); data += 2;

    if (m_extraFmtData.size() > 0) {
        memcpy(data, m_extraFmtData.data(), m_extraFmtData.size());
        data += m_extraFmtData.size();
        data += m_extraFmtData.size() % 2;
    }

    memcpy(data, "data", 4); data += 4;
    littleEndianWrite<std::uint32_t>(m_wavedata.size(), data); data += 4; // Padding byte

    memcpy(data, (const char*)(m_wavedata.data()), m_wavedata.size());
    data += m_wavedata.size();
    data += m_wavedata.size() % 2; // Padding byte

    // This chunk is needed by the specs every time
    // the format is not PCM (i.e. FLOAT, ADPCM, whatever...) 
    memcpy(data, "fact", 4); data += 4;
    littleEndianWrite<std::uint32_t>(4, data); data += 4;
    littleEndianWrite<std::uint32_t>(1, data); data += 4;

    return output;
}

void Wave::writeToStream(std::ostream& stream) const {
    auto wavefile = getWaveFile();
    stream.write((const char*)wavefile.data(), wavefile.size());
}
#include <dmusic/dls/Instrument.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic::Riff;
using namespace DirectMusic::DLS;

Region::Region(const Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "rgn ")
        throw DirectMusic::InvalidChunkException("LIST rgn", c.getId() + " " + c.getListId());

    for(const Chunk& subchunk: c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        if(id == "rgnh") {
            m_rgnHeader = RegionHeader(subchunk.getData().data());
        } else if(id == "LIST" && subchunk.getListId() == "lart") {
            for (Chunk art1ck : subchunk.getSubchunks()) {
                m_articulators.push_back(Articulator(art1ck));
            }
        } else if (id == "wlnk") {
            m_waveLink = WaveLink(subchunk.getData().data());
        } else if (id == "wsmp") {
            m_wavesample = Wavesample(subchunk.getData().data());
            if (m_wavesample.cSampleLoops > 0) {
                const std::uint8_t *data = subchunk.getData().data() + m_wavesample.cbSize;
                for (int i = 0; i < m_wavesample.cSampleLoops; i++) {
                    m_loops.push_back(WavesampleLoop(data));
                    data += sizeof(WavesampleLoop);
                }
            }
        }
    }
}
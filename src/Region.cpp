#include <dmusic/dls/Instrument.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic::Riff;
using namespace DirectMusic::DLS;

Region::Region(Chunk& c) {
    if (c.getId().compare("LIST") != 0 || c.getListId().compare("rgn ") != 0)
        throw DirectMusic::InvalidChunkException("LIST rgn", c.getId() + " " + c.getListId());

    for(Chunk subchunk: c.getSubchunks()) {
        if(!subchunk.getId().compare("rgnh")) {
            RegionHeader *header = (RegionHeader*)subchunk.getData().data();
            m_rgnHeader = *header;
        } else if(!subchunk.getId().compare("LIST") && !subchunk.getListId().compare("lart")) {
            for (Chunk art1ck : subchunk.getSubchunks()) {
                m_articulators.push_back(Articulator(art1ck));
            }
        } else if (!subchunk.getId().compare("wlnk")) {
            WaveLink *wlnk = (WaveLink*)subchunk.getData().data();
            m_waveLink = *wlnk;
        } else if (!subchunk.getId().compare("wsmp")) {
            Wavesample *wsmp = (Wavesample*)subchunk.getData().data();
            m_wavesample = *wsmp;
            if (m_wavesample.cSampleLoops > 0) {
                WavesampleLoop *loops = (WavesampleLoop*)(subchunk.getData().data() + m_wavesample.cbSize);
                for (int i = 0; i < m_wavesample.cSampleLoops; i++) {
                    m_loops.push_back(loops[i]);
                }
            }
        }
    }
}

const RegionHeader& Region::getRegionHeader() const {
    return m_rgnHeader;
}

const WaveLink& Region::getWaveLink() const {
    return m_waveLink;
}

const Wavesample& Region::getWavesample() const {
    return m_wavesample;
}

const std::vector<Articulator> Region::getArticulators() const {
    return m_articulators;
}

const std::vector<WavesampleLoop> Region::getWavesampleLoops() const {
    return m_loops;
}

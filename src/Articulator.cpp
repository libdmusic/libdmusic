#include <dmusic/dls/Instrument.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic::Riff;
using namespace DirectMusic::DLS;

Articulator::Articulator(Chunk& c) {
    if (c.getId().compare("art1") != 0)
        throw DirectMusic::InvalidChunkException("art1", c.getId());

    ArticulatorHeader *header = (ArticulatorHeader*)c.getData().data();
    ConnectionBlock *blocks = (ConnectionBlock*)(c.getData().data() + header->cbSize);
    for (int i = 0; i < header->cConnectionBlocks; i++) {
        m_connectionBlocks.push_back(blocks[i]);
    }
}

const std::vector<ConnectionBlock>& Articulator::getConnectionBlocks() const {
    return m_connectionBlocks;
}

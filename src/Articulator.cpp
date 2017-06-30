#include <dmusic/dls/Instrument.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic::Riff;
using namespace DirectMusic::DLS;

Articulator::Articulator(const Chunk& c) {
    if (c.getId() != "art1")
        throw DirectMusic::InvalidChunkException("art1", c.getId());

    const std::uint8_t *data = c.getData().data();
    ArticulatorHeader header(data);
    data += header.cbSize;
    ConnectionBlock block;
    for (int i = 0; i < header.cConnectionBlocks; i++) {
        block = ConnectionBlock(data);
        m_connectionBlocks.push_back(block);
        data += sizeof(ConnectionBlock);
    }
}

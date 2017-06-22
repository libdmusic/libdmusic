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
            m_rangeKey = header->RangeKey;
            m_rangeVelocity = header->RangeVelocity;
            m_fusOptions = header->fusOptions;
            m_usKeyGroup = header->usKeyGroup;
        } else if(!subchunk.getId().compare("LIST") && !subchunk.getListId().compare("lart")) {

        }
    }
}

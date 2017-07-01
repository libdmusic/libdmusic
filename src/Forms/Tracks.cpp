#include <dmusic/Common.h>
#include <dmusic/Forms.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic;
using namespace DirectMusic::Riff;

TrackForm::TrackForm(const Chunk& c) {
    if (c.getId() != "RIFF" || c.getListId() != "DMTK")
        throw DirectMusic::InvalidChunkException("RIFF DMTK", c.getId() + " " + c.getListId());

    for(Chunk subchunk : c.getSubchunks()) {
        std::string id = subchunk.getId();
        if(id == "guid") {
            m_guid = GUID(subchunk.getData().data());
        } else if(id == "vers") {
            m_version = DMUS_IO_VERSION(subchunk.getData().data());
        } else if (id == "trkh") {
            m_header = DMUS_IO_TRACK_HEADER(subchunk.getData().data());
        } else if(id == "trkx") {
            m_flags = std::make_shared<DMUS_IO_TRACK_EXTRAS_HEADER>(DMUS_IO_TRACK_EXTRAS_HEADER(subchunk.getData().data()));
        } else if(id == "LIST") {
            std::string listid = subchunk.getListId();
            if(listid == "UNFO") {
                m_unfo = Unfo(subchunk);
            }
        }
    }
}

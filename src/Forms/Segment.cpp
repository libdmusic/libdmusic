#include <dmusic/Forms.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic;
using namespace DirectMusic::Riff;

SegmentForm::SegmentForm(const Chunk& c) {
    if (c.getId() != "RIFF" || c.getListId() != "DMSG")
        throw DirectMusic::InvalidChunkException("RIFF DMSG", c.getId() + " " + c.getListId());

    for (const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        if (id == "segh") {
            m_header = DMUS_IO_SEGMENT_HEADER(subchunk.getData().data());
        } else if (id == "guid") {
            m_guid = GUID(subchunk.getData().data());
        } else if (id == "vers") {
            m_version = DMUS_IO_VERSION(subchunk.getData().data());
        } else if (id == "LIST") {
            std::string listid = subchunk.getListId();
            if (listid == "UNFO") {
                m_unfo = Unfo(subchunk);
            } else if (listid == "trkl") {
                for (Chunk track : subchunk.getSubchunks()) {
                    if (track.getId() == "RIFF" && track.getListId() == "DMTK") {
                        m_tracks.push_back(TrackForm(track));
                    }
                }
            }
        } else {
            // TODO: Read Toolgraph, Audiopath and Container
        }
    }
}
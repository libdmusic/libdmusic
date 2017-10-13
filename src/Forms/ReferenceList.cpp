#include <dmusic/Forms.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic;
using namespace DirectMusic::Riff;

ReferenceList::ReferenceList(const Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "DMRF")
        throw DirectMusic::InvalidChunkException("LIST DMRF", c.getId() + " " + c.getListId());

    for(const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        if(id == "refh") {
            m_header = DMUS_IO_REFERENCE(subchunk.getData().data());
        } else if(id == "guid") {
            m_guid = GUID(subchunk.getData().data());
        } else if(id == "date") {
            // Let's ignore it for now...
        } else if(id == "name") {
            m_name = std::string(utf16_to_utf8((const std::uint16_t*)subchunk.getData().data()));
        } else if(id == "file") {
            m_file = std::string(utf16_to_utf8((const std::uint16_t*)subchunk.getData().data()));
        } else if(id == "catg") {
            m_category = std::string(utf16_to_utf8((const std::uint16_t*)subchunk.getData().data()));
        } else if(id == "vers") {
            m_version = DMUS_IO_VERSION(subchunk.getData().data());
        }
    }
}

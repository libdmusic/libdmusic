#include <dmusic/Common.h>
#include <dmusic/Forms.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic;
using namespace DirectMusic::Riff;

ReferenceList::ReferenceList(const Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "DMRF")
        throw DirectMusic::InvalidChunkException("LIST DMRF", c.getId() + " " + c.getListId());

    for(Chunk subchunk : c.getSubchunks()) {
        std::string id = subchunk.getId();
        if(id == "refh") {
            m_header = DMUS_IO_REFERENCE(subchunk.getData().data());
        } else if(id == "guid") {
            m_guid = GUID(subchunk);
        } else if(id == "date") {
            // Let's ignore it for now...
        } else if(id == "name") {
            m_name = std::wstring((const wchar_t *)subchunk.getData().data());
        } else if(id == "file") {
            m_file = std::wstring((const wchar_t *)subchunk.getData().data());
        } else if(id == "catg") {
            m_category = std::wstring((const wchar_t *)subchunk.getData().data());
        } else if(id == "vers") {
            m_version = DMUS_IO_VERSION(subchunk.getData().data());
        }
    }
}

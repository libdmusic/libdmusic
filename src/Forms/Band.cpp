#include <dmusic/Forms.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic;
using namespace DirectMusic::Riff;

BandInstrument::BandInstrument(const Chunk& c)
    : m_reference(nullptr)
{
    if (c.getId() != "LIST" || c.getListId() != "lbin")
        throw DirectMusic::InvalidChunkException("LIST lbin", c.getId() + " " + c.getListId());

    for(const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        if(id == "bins") {
            m_header = DMUS_IO_INSTRUMENT(subchunk.getData().data());
        } else if(id == "LIST" && subchunk.getListId() == "DMRF") {
            m_reference = std::make_shared<ReferenceList>(subchunk);
        }
    }
}

BandForm::BandForm(const Chunk& c) {
    if (c.getId() != "RIFF" || c.getListId() != "DMBD")
        throw DirectMusic::InvalidChunkException("RIFF DMBD", c.getId() + " " + c.getListId());

    for(const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        if(id == "guid") {
            m_guid = GUID(subchunk.getData().data());
        } else if(id == "vers") {
            m_version = DMUS_IO_VERSION(subchunk.getData().data());
        } else if(id == "LIST") {
            std::string listid = subchunk.getListId();
            if(listid == "UNFO") {
                m_unfo = Unfo(subchunk);
            } else if(listid == "lbil") {
                for(Chunk inst : subchunk.getSubchunks()) {
                    if(inst.getId() == "LIST" && inst.getListId() == "lbin") {
                        m_instruments.push_back(BandInstrument(inst));
                    }
                }
            }
        }
    }
}

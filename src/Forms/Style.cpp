#include <dmusic/Forms.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic;
using namespace DirectMusic::Riff;

/**
* Loads a series of structures of type T into a vector from a memory buffer
* The first 4 bytes represent the size of the structure to load
* The number of structures to load is calculated from the size of the buffer
* and the size of the data.
* @param subchunkData A memory buffer
* @param output The vector where the structures should be stored
*/
template<typename T>
static void loadData(const std::vector<std::uint8_t>& subchunkData, std::vector<T>& output) {
    const std::uint8_t *data = subchunkData.data();
    std::uint32_t structSize = littleEndianRead<std::uint32_t>(data);
    data += 4;
    int numElements = (subchunkData.size() - 4) / structSize;
    for (int i = 0; i < numElements; i++) {
        output.push_back(T(data));
        data += structSize;
    }
}

StylePart::StylePart(const Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "part")
        throw DirectMusic::InvalidChunkException("LIST part", c.getId() + " " + c.getListId());

    for (const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        const std::vector<std::uint8_t>& subchunkData = subchunk.getData();
        if (id == "prth") {
            m_header = DMUS_IO_STYLEPART(subchunkData.data());
        } else if(id == "note") {
            loadData<DMUS_IO_STYLENOTE>(subchunkData, m_notes);
        } else if (id == "crve") {
            loadData<DMUS_IO_STYLECURVE>(subchunkData, m_curves);
        } else if (id == "mrkr") {
            loadData<DMUS_IO_STYLEMARKER>(subchunkData, m_markers);
        } else if (id == "rsln") {
            loadData<DMUS_IO_STYLERESOLUTION>(subchunkData, m_resolutions);
        } else if (id == "anpn") {
            loadData<DMUS_IO_STYLE_ANTICIPATION>(subchunkData, m_anticipations);
        } else if (id == "LIST") {
            std::string listid = subchunk.getListId();
            if (listid == "UNFO") {
                m_unfo = Unfo(subchunk);
            }
        }
    }
}

Pattern::Pattern(const Chunk& c)
    : m_motifSettings(nullptr),
    m_band(nullptr)
{
    if (c.getId() != "LIST" || c.getListId() != "pttn")
        throw DirectMusic::InvalidChunkException("LIST pttn", c.getId() + " " + c.getListId());

    for (const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        const std::uint8_t *data = subchunk.getData().data();
        const std::uint8_t *start = data;
        if (id == "ptnh") {
            m_header = DMUS_IO_PATTERN(data);
        } else if (id == "rhtm") {
            while (data - start < subchunk.getData().size()) {
                m_rhythms.push_back(littleEndianRead<std::uint16_t>(data));
                data += 2;
            }
        } else if(id == "mtfs") {
            m_motifSettings = std::make_shared<DMUS_IO_MOTIFSETTINGS>(data);
        } else if (id == "LIST") {
            std::string listid = subchunk.getListId();
            if (listid == "UNFO") {
                m_unfo = Unfo(subchunk);
            } else if (listid == "part") {
                m_parts.push_back(StylePart(subchunk));
            } else if (listid == "pref") {
                Unfo unfo;
                DMUS_IO_PARTREF partref;
                for (Chunk pref : subchunk.getSubchunks()) {
                    if (pref.getId() == "prfc") {
                       partref = DMUS_IO_PARTREF(pref.getData().data());
                    } else if (pref.getId() == "LIST" && pref.getListId() == "UNFO") {
                        unfo = Unfo(pref);
                    }
                }
                m_partrefs.push_back(std::make_pair(partref, unfo));
            }
        } else if (id == "RIFF") {
            if (subchunk.getListId() == "DMBD") {
                m_band = std::make_shared<BandForm>(subchunk);
            }
        }
    }
}

StyleForm::StyleForm(const Chunk& c) {
    if (c.getId() != "RIFF" || c.getListId() != "DMST")
        throw DirectMusic::InvalidChunkException("RIFF DMST", c.getId() + " " + c.getListId());

    for (const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        const std::uint8_t *data = subchunk.getData().data();
        const std::uint8_t *start = data;
        if (id == "styh") {
            m_header = DMUS_IO_STYLE(data);
        } else if (id == "guid") {
            m_guid = GUID(data);
        } else if (id == "vers") {
            m_version = DMUS_IO_VERSION(data);
        } else if (id == "LIST") {
            std::string listid = subchunk.getListId();
            if (listid == "UNFO") {
                m_unfo = Unfo(subchunk);
            } else if (listid == "part") {
                m_parts.push_back(StylePart(subchunk));
            } else if (listid == "pttn") {
                m_patterns.push_back(Pattern(subchunk));
            } else if (listid == "prrf") {
                for (Chunk prrf : subchunk.getSubchunks()) {
                    if (prrf.getId() == "LIST" && prrf.getListId() == "DMRF") {
                        m_references.push_back(ReferenceList(prrf));
                    }
                }
            }
        } else if (id == "RIFF") {
            if (subchunk.getListId() == "DMBD") {
                m_bands.push_back(BandForm(subchunk));
            }
        }
    }
}
#include <dmusic/Forms.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic;
using namespace DirectMusic::Riff;

Chord DirectMusic::readChord(const Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "chrd")
        throw DirectMusic::InvalidChunkException("LIST chrd", c.getId() + " " + c.getListId());

    std::string name;
    std::vector<std::uint16_t> indexes;

    for (const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        const std::uint8_t *data = subchunk.getData().data();
        if (id == "UNAM") {
            name = std::string(utf16_to_utf8((const std::uint16_t*)data));
        } else if (id == "sbcn") {
            const std::uint8_t *start = data;
            while (data - start < subchunk.getData().size()) {
                indexes.push_back(littleEndianRead<std::uint16_t>(data));
                data += 2;
            }
        }
    }

    return Chord(name, indexes);
}

static void readChordList(const Chunk& c, std::vector<Chord>& vec) {
    for(Chunk chrd : c.getSubchunks()) {
        if (chrd.getId() == "LIST" && chrd.getListId() == "chrd") {
            vec.push_back(readChord(chrd));
        }
    }
}

ChordEntry::ChordEntry(const Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "choe")
        throw DirectMusic::InvalidChunkException("LIST choe", c.getId() + " " + c.getListId());

    for (const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        if (id == "cheh") {
            m_header = DMUS_IO_CHORDENTRY(subchunk.getData().data());
        } else if (id == "ncsq") {
            const std::uint8_t *data = subchunk.getData().data();
            const std::uint8_t *start = data;
            std::uint16_t structSize = littleEndianRead<std::uint16_t>(data);
            data += 2;
            while (data - start < subchunk.getData().size()) {
                m_nextChords.push_back(DMUS_IO_NEXTCHORD(data));
                data += structSize;
            }
        } else if (id == "LIST" && subchunk.getListId() == "chrd") {
            m_data = readChord(subchunk);
        }
    }
}

Signpost::Signpost(const Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "spst")
        throw DirectMusic::InvalidChunkException("LIST spsq", c.getId() + " " + c.getListId());

    for(const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        if(id == "spsh") {
            m_header = DMUS_IO_CHORDMAP_SIGNPOST(c.getData().data());
        } else if(id == "LIST") {
            std::string listId = subchunk.getListId();
            if(listId == "chrd") {
                m_chordData = readChord(subchunk);
            } else if(listId == "cade") {
                readChordList(subchunk, m_cadeChords);
            }
        }
     }
}

ChordmapForm::ChordmapForm(const Chunk& c) {
    if (c.getId() != "RIFF" || c.getListId() != "DMPR")
        throw DirectMusic::InvalidChunkException("RIFF DMPR", c.getId() + " " + c.getListId());

    for(const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        if(id == "perh") {
            m_header = DMUS_IO_CHORDMAP(c.getData().data());
        } else if(id == "guid") {
            m_guid = GUID(c.getData().data());
        } else if(id == "vers") {
            m_version = DMUS_IO_VERSION(c.getData().data());
        } else if(id == "chdt") {
            const std::uint8_t *data = subchunk.getData().data();
            const std::uint8_t *start = data;
            std::uint16_t structSize = littleEndianRead<std::uint16_t>(data);
            data += 2;
            while (data - start < subchunk.getData().size()) {
                m_subchords.push_back(DMUS_IO_CHORDMAP_SUBCHORD(data));
                data += structSize;
            }
        } else if(id == "LIST") {
            std::string listid = subchunk.getListId();
            if(listid == "UNFO") {
                m_unfo = Unfo(subchunk);
            } else if(listid == "chpl") {
                readChordList(subchunk, m_chordPalette);
            } else if(listid == "cmap") {
                for(Chunk entry : subchunk.getSubchunks()) {
                    if(entry.getId() == "LIST" && entry.getListId() == "choe") {
                        m_entries.push_back(ChordEntry(entry));
                    }
                }
            } else if(listid == "spsq") {
                for(Chunk signpost : subchunk.getSubchunks()) {
                    if(signpost.getId() == "LIST" && signpost.getListId() == "spst") {
                        m_signposts.push_back(Signpost(signpost));
                    }
                }
            }
        }
    }
}

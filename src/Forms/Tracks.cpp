#include <dmusic/Common.h>
#include <dmusic/Forms.h>
#include <dmusic/Tracks.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic;
using namespace DirectMusic::Riff;

TrackForm::TrackForm(const Chunk& c)
    : m_flags(nullptr) {
    if (c.getId() != "RIFF" || c.getListId() != "DMTK")
        throw DirectMusic::InvalidChunkException("RIFF DMTK", c.getId() + " " + c.getListId());

    for (Chunk subchunk : c.getSubchunks()) {
        std::string id = subchunk.getId();
        if (id == "guid") {
            m_guid = GUID(subchunk.getData().data());
        } else if (id == "vers") {
            m_version = DMUS_IO_VERSION(subchunk.getData().data());
        } else if (id == "trkh") {
            m_header = DMUS_IO_TRACK_HEADER(subchunk.getData().data());
        } else if (id == "trkx") {
            m_flags = std::make_shared<DMUS_IO_TRACK_EXTRAS_HEADER>(DMUS_IO_TRACK_EXTRAS_HEADER(subchunk.getData().data()));
        } else if (id == "LIST") {
            std::string listid = subchunk.getListId();
            if (listid == "UNFO") {
                m_unfo = Unfo(subchunk);
            }
        }
    }
}

static BandItem parseBandItem(const Chunk& c) {
    DMUS_IO_BAND_ITEM_HEADER2 header;
    for (Chunk subchunk : c.getSubchunks()) {
        std::string id = subchunk.getId();
        if (id == "bdih") {
            DMUS_IO_BAND_ITEM_HEADER h(subchunk.getData().data());
            header.lBandTimeLogical = h.lBandTime;
            header.lBandTimePhysical = h.lBandTime;
        } else if (id == "bd2h") {
            header = DMUS_IO_BAND_ITEM_HEADER2(subchunk.getData().data());
        } else if (id == "RIFF" && subchunk.getListId() == "DMBD") {
            return std::make_tuple(header, BandForm(subchunk));
        }
    }

    throw "Invalid band item";
}

BandTrack::BandTrack(const Chunk& c) {
    if (c.getId() != "RIFF" || c.getListId() != "DMBT")
        throw DirectMusic::InvalidChunkException("RIFF DMBT", c.getId() + " " + c.getListId());

    for (Chunk subchunk : c.getSubchunks()) {
        std::string id = subchunk.getId();
        if (id == "guid") {
            m_guid = GUID(subchunk.getData().data());
        } else if (id == "vers") {
            m_version = DMUS_IO_VERSION(subchunk.getData().data());
        } else if (id == "bdth") {
            m_header = DMUS_IO_BAND_TRACK_HEADER(subchunk.getData().data());
        } else if (id == "LIST") {
            std::string listid = subchunk.getListId();
            if (listid == "UNFO") {
                m_unfo = Unfo(subchunk);
            } else if (listid == "lbdl") {
                for (Chunk band : subchunk.getSubchunks()) {
                    if (band.getId() == "LIST" && band.getListId() == "lbnd") {
                        m_bands.push_back(parseBandItem(band));
                    }
                }
            }
        }
    }
}

ChordTrack::ChordTrack(const Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "cord")
        throw DirectMusic::InvalidChunkException("LIST cord", c.getId() + " " + c.getListId());

    for (Chunk subchunk : c.getSubchunks()) {
        std::string id = subchunk.getId();
        if (id == "crdh") {
            m_header = littleEndianRead<std::uint32_t>(subchunk.getData().data());
        } else if (id == "crdb") {
            const std::uint8_t *data = c.getData().data();
            std::uint16_t chordSize = littleEndianRead<std::uint16_t>(data);
            data += 2;
            m_chord = DMUS_IO_CHORD(data);
            data += chordSize;
            std::uint16_t subchordNum = littleEndianRead<std::uint16_t>(data);
            data += 2;
            std::uint16_t subchordSize = littleEndianRead<std::uint16_t>(data);
            data += 2;
            for (int i = 0; i < subchordNum; i++) {
                m_subchords.push_back(DMUS_IO_SUBCHORD(data));
            }
        }
    }
}

static Chordmap parseChordmap(const Chunk& c) {
    std::uint16_t stamp;
    for (Chunk subchunk : c.getSubchunks()) {
        std::string id = subchunk.getId();
        if (id == "stmp") {
            stamp = littleEndianRead<std::uint16_t>(subchunk.getData().data());
        } else if (id == "LIST" && subchunk.getListId() == "DMRF") {
            return std::make_tuple(stamp, ReferenceList(subchunk));
        }
    }

    throw "Invalid band item";
}

ChordmapTrack::ChordmapTrack(const Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "pftr")
        throw DirectMusic::InvalidChunkException("LIST pftr", c.getId() + " " + c.getListId());

    for (Chunk subchunk : c.getSubchunks()) {
        if (subchunk.getId() == "LIST" && subchunk.getListId() == "pfrf") {
            m_chordmaps.push_back(parseChordmap(subchunk));
        }
    }
}

CommandTrack::CommandTrack(const Chunk& c) {
    if (c.getId() != "cmnd")
        throw DirectMusic::InvalidChunkException("cmnd", c.getId() + " " + c.getListId());
    const std::uint8_t *data = c.getData().data();
    int offset = 0;
    std::uint16_t cmdSize = littleEndianRead<std::uint16_t>(data + offset);
    offset += 2;
    while (offset < c.getData().size()) {
        m_commands.push_back(DMUS_IO_COMMAND(data + offset));
        offset += cmdSize;
    }
}

static LyricsEvent readLyricsEvent(const Chunk& c) {
    DMUS_IO_LYRICSTRACK_EVENTHEADER header;
    for (Chunk subchunk : c.getSubchunks()) {
        std::string id = subchunk.getId();
        if (id == "stmp") {
            header = DMUS_IO_LYRICSTRACK_EVENTHEADER(subchunk.getData().data());
        } else if (id == "lyrn") {
            return std::make_tuple(header, std::wstring((const wchar_t *)subchunk.getData().data()));
        }
    }

    throw "Invalid band item";
}

LyricsTrack::LyricsTrack(const Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "lyrt")
        throw DirectMusic::InvalidChunkException("LIST lyrt", c.getId() + " " + c.getListId());

    for (Chunk subchunk : c.getSubchunks()) {
        if (subchunk.getId() == "LIST" && subchunk.getListId() == "lyrt") {
            m_lyrics.push_back(readLyricsEvent(subchunk));
        }
    }
}
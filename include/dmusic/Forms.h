#pragma once

#include <cstdint>
#include <vector>
#include <tuple>
#include <string>
#include <memory>
#include "Riff.h"
#include "Structs.h"

namespace DirectMusic {
    typedef std::tuple<std::wstring, std::vector<std::uint16_t>> Chord;
    Chord readChord(const DirectMusic::Riff::Chunk& chunk);

    class ChordEntry {
    public:
        ChordEntry(const DirectMusic::Riff::Chunk& chunk);
        const DMUS_IO_CHORDENTRY& getHeader() const { return m_header; }
        const Chord& getChordData() const { return m_data; }
        const std::vector<DMUS_IO_NEXTCHORD>& getNextChords() const { return m_nextChords; }

    private:
        DMUS_IO_CHORDENTRY m_header;
        std::vector<DMUS_IO_NEXTCHORD> m_nextChords;
        Chord m_data;
    };

    class Signpost {
    public:
        Signpost(const DirectMusic::Riff::Chunk& chunk);
        const DMUS_IO_CHORDMAP_SIGNPOST& getHeader() const { return m_header; }
        const Chord& getChordData() const { return m_chordData; }
        const std::vector<Chord>& getCadenceChords() const { return m_cadeChords; }

    private:
        DMUS_IO_CHORDMAP_SIGNPOST m_header;
        Chord m_chordData;
        std::vector<Chord> m_cadeChords;
    };

    /// A chordmap is a collection of chords that provides multiple potential chord progressions to a musical piece.
    /// Chord progressions are generated from a chordmap and inserted into the chord track of a segment, either at design time or at run time.
    class ChordmapForm {
    public:
        ChordmapForm(const DirectMusic::Riff::Chunk& chunk);
        const DMUS_IO_CHORDMAP& getHeader() const { return m_header; }
        const GUID& getGuid() const { return m_guid; }
        const DirectMusic::Riff::Unfo& getInfo() const { return m_unfo; }
        const std::vector<DMUS_IO_CHORDMAP_SUBCHORD>& getChordData() const { return m_subchords; }
        const std::vector<Chord>& getChordPalette() const { return m_chordPalette; }
        const std::vector<ChordEntry>& getChordEntries() const { return m_entries; }
        const std::vector<Signpost>& getSignPosts() const { return m_signposts; }
        const DMUS_IO_VERSION& getVersion() const { return m_version; }

    private:
        DMUS_IO_CHORDMAP m_header;
        GUID m_guid;
        DirectMusic::Riff::Unfo m_unfo;
        std::vector<DMUS_IO_CHORDMAP_SUBCHORD> m_subchords;
        std::vector<Chord> m_chordPalette;
        std::vector<ChordEntry> m_entries;
        std::vector<Signpost> m_signposts;
        DMUS_IO_VERSION m_version;
    };

    /// The reference list chunk contains information about a reference to an object in another file.
    /// For example, a band object might contain a reference to a DLS collection in a separate file.
    /// This subchunk is used in many different chunks.
    class ReferenceList {
    public:
        ReferenceList(const DirectMusic::Riff::Chunk& chunk);
        const DMUS_IO_REFERENCE& getHeader() const { return m_header; }
        const GUID& getGuid() const { return m_guid; }
        //const FILETIME& getDate() const { return m_date; }
        const std::wstring getName() const { return m_name; }
        const std::wstring getFile() const { return m_file; }
        const std::wstring getCategory() const { return m_category; }
        const DMUS_IO_VERSION& getVersion() const { return m_version; }

    private:
        DMUS_IO_REFERENCE m_header;
        GUID m_guid;
        std::wstring m_name, m_file, m_category;
        DMUS_IO_VERSION m_version;
    };

    class BandInstrument {
    public:
        BandInstrument(const DirectMusic::Riff::Chunk& chunk);
        const DMUS_IO_INSTRUMENT& getHeader() const { return m_header; }
        const std::shared_ptr<ReferenceList>& getReference() const { return m_reference; }

    private:
        DMUS_IO_INSTRUMENT m_header;
        std::shared_ptr<ReferenceList> m_reference;
    };

    class BandForm {
    public:
        BandForm(const DirectMusic::Riff::Chunk& chunk);
        const GUID& getGuid() const { return m_guid; }
        const DMUS_IO_VERSION& getVersion() const { return m_version; }
        const DirectMusic::Riff::Unfo& getInfo() const { return m_unfo; }
        const std::vector<BandInstrument>& getInstruments() const { return m_instruments; }

    private:
        GUID m_guid;
        DMUS_IO_VERSION m_version;
        DirectMusic::Riff::Unfo m_unfo;
        std::vector<BandInstrument> m_instruments;
    };

    class SubtrackForm {};

    /// The track form contains information about a single track.
    /// It can be embedded in a Segment Form or stored in its own file.
    class TrackForm {
    public:
        TrackForm(const DirectMusic::Riff::Chunk& chunk);
        const GUID& getGuid() const { return m_guid; }
        const DMUS_IO_VERSION& getVersion() const { return m_version; }
        const DirectMusic::Riff::Unfo& getInfo() const { return m_unfo; }
        const DMUS_IO_TRACK_HEADER& getHeader() const { return m_header; }
        const std::shared_ptr<DMUS_IO_TRACK_EXTRAS_HEADER>& getTrackFlags() const { return m_flags; }
        const SubtrackForm& getData() const { return m_data; }

    private:
        GUID m_guid;
        DMUS_IO_VERSION m_version;
        DirectMusic::Riff::Unfo m_unfo;
        DMUS_IO_TRACK_HEADER m_header;
        std::shared_ptr<DMUS_IO_TRACK_EXTRAS_HEADER> m_flags;
        SubtrackForm m_data;
    };

    class SegmentForm {
    public:
        SegmentForm(const DirectMusic::Riff::Chunk& chunk);
        const GUID& getGuid() const { return m_guid; }
        const DMUS_IO_VERSION& getVersion() const { return m_version; }
        const DirectMusic::Riff::Unfo& getInfo() const { return m_unfo; }
        const DMUS_IO_SEGMENT_HEADER& getHeader() const { return m_header; }
        const std::vector<TrackForm>& getTracks() const { return m_tracks; }

    private:
        GUID m_guid;
        DMUS_IO_VERSION m_version;
        DirectMusic::Riff::Unfo m_unfo;
        DMUS_IO_SEGMENT_HEADER m_header;
        std::vector<TrackForm> m_tracks;
    };
}

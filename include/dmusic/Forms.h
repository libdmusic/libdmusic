#pragma once

#include <cstdint>
#include <vector>
#include <tuple>
#include <string>
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
}

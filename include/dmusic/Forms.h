#pragma once

#include <cstdint>
#include <vector>
#include <utility>
#include <string>
#include <memory>
#include "Common.h"
#include "Riff.h"
#include "Structs.h"

namespace DirectMusic {
    typedef std::pair<std::string, std::vector<std::uint16_t>> Chord;
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
        const std::string getName() const { return m_name; }
        const std::string getFile() const { return m_file; }
        const std::string getCategory() const { return m_category; }
        const DMUS_IO_VERSION& getVersion() const { return m_version; }

    private:
        DMUS_IO_REFERENCE m_header;
        GUID m_guid;
        std::string m_name, m_file, m_category;
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

        bool operator==(const BandForm& a) const {
            return a.m_guid == m_guid &&
                a.m_version == m_version &&
                a.m_unfo == m_unfo &&
                a.m_instruments.size() == m_instruments.size();
        }

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
        const std::shared_ptr<SubtrackForm> getData() const { return m_data; }

    private:
        GUID m_guid;
        DMUS_IO_VERSION m_version;
        DirectMusic::Riff::Unfo m_unfo;
        DMUS_IO_TRACK_HEADER m_header;
        std::shared_ptr<DMUS_IO_TRACK_EXTRAS_HEADER> m_flags;
        std::shared_ptr<SubtrackForm> m_data;
    };

    class SegmentForm {
    public:
        SegmentForm(const DirectMusic::Riff::Chunk& chunk);
        const GUID& getGuid() const { return m_guid; }
        const DMUS_IO_VERSION& getVersion() const { return m_version; }
        const DirectMusic::Riff::Unfo& getInfo() const { return m_unfo; }
        const DMUS_IO_SEGMENT_HEADER& getHeader() const { return m_header; }
        const std::vector<TrackForm>& getTracks() const { return m_tracks; }

        bool operator==(const SegmentForm& a) const {
            return a.m_guid == m_guid &&
                a.m_version == m_version &&
                a.m_unfo == m_unfo &&
                a.m_header == m_header &&
                a.m_tracks.size() == m_tracks.size();
        }

    private:
        GUID m_guid;
        DMUS_IO_VERSION m_version;
        DirectMusic::Riff::Unfo m_unfo;
        DMUS_IO_SEGMENT_HEADER m_header;
        std::vector<TrackForm> m_tracks;
    };

    class StylePart {
    public:
        StylePart() {};
        StylePart(const DirectMusic::Riff::Chunk& chunk);
        const DirectMusic::Riff::Unfo& getInfo() const { return m_unfo; }
        const DMUS_IO_STYLEPART& getHeader() const { return m_header; }
        const std::vector<DMUS_IO_STYLENOTE>& getNotes() const { return m_notes; }
        const std::vector<DMUS_IO_STYLECURVE>& getCurves() const { return m_curves; }
        const std::vector<DMUS_IO_STYLEMARKER>& getMarkers() const { return m_markers; }
        const std::vector<DMUS_IO_STYLERESOLUTION>& getResolutions() const { return m_resolutions; }
        const std::vector<DMUS_IO_STYLE_ANTICIPATION>& getAnticipations() const { return m_anticipations; }

    private:
        DirectMusic::Riff::Unfo m_unfo;
        DMUS_IO_STYLEPART m_header;
        std::vector<DMUS_IO_STYLENOTE> m_notes;
        std::vector<DMUS_IO_STYLECURVE> m_curves;
        std::vector<DMUS_IO_STYLEMARKER> m_markers;
        std::vector<DMUS_IO_STYLERESOLUTION> m_resolutions;
        std::vector<DMUS_IO_STYLE_ANTICIPATION> m_anticipations;
    };


    typedef std::pair<DMUS_IO_PARTREF, DirectMusic::Riff::Unfo> PartReference;
    class Pattern {
    public:
        Pattern(const DirectMusic::Riff::Chunk& chunk);
        const DMUS_IO_PATTERN& getHeader() const { return m_header; }
        const DirectMusic::Riff::Unfo& getInfo() const { return m_unfo; }
        const std::vector<std::uint16_t>& getRhythms() const { return m_rhythms; }
        const std::shared_ptr<DMUS_IO_MOTIFSETTINGS>& getMotifSettings() const { return m_motifSettings; }
        const std::shared_ptr<BandForm>& getBand() const { return m_band; }
        const std::vector<PartReference>& getPartReferences() const { return m_partrefs; }

        /// Only used in Pattern Tracks
        const std::vector<StylePart> getParts() const { return m_parts; }

    private:
        DMUS_IO_PATTERN m_header;
        DirectMusic::Riff::Unfo m_unfo;
        std::vector<std::uint16_t> m_rhythms;
        std::shared_ptr<DMUS_IO_MOTIFSETTINGS> m_motifSettings;
        std::shared_ptr<BandForm> m_band;
        std::vector<PartReference> m_partrefs;
        std::vector<StylePart> m_parts;
    };

    class StyleForm {
    public:
        StyleForm(const DirectMusic::Riff::Chunk& chunk);
        const GUID& getGuid() const { return m_guid; }
        const DMUS_IO_VERSION& getVersion() const { return m_version; }
        const DirectMusic::Riff::Unfo& getInfo() const { return m_unfo; }
        const DMUS_IO_STYLE& getHeader() const { return m_header; }
        const std::vector<StylePart>& getParts() const { return m_parts; }
        const std::vector<Pattern>& getPatterns() const { return m_patterns; }
        const std::vector<BandForm>& getBands() const { return m_bands; }
        const std::vector<ReferenceList>& getChordmapReferences() const { return m_references; }

        bool operator==(const StyleForm& a) {
            return a.m_guid == m_guid &&
                a.m_version == m_version &&
                a.m_unfo == m_unfo &&
                a.m_header == m_header &&
                a.m_parts.size() == m_parts.size() &&
                a.m_patterns.size() == m_patterns.size() &&
                a.m_bands.size() == m_bands.size() &&
                a.m_references.size() == m_references.size();
        }

    private:
        GUID m_guid;
        DMUS_IO_VERSION m_version;
        DirectMusic::Riff::Unfo m_unfo;
        DMUS_IO_STYLE m_header;
        std::vector<StylePart> m_parts;
        std::vector<Pattern> m_patterns;
        std::vector<BandForm> m_bands;
        std::vector<ReferenceList> m_references;
    };
}

namespace std {
    template<> struct hash<DirectMusic::BandForm> {
        typedef DirectMusic::BandForm argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const noexcept {
            return std::hash<DirectMusic::GUID>{}(s.getGuid());
        }
    };

    template<> struct hash<DirectMusic::SegmentForm> {
        typedef DirectMusic::SegmentForm argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const noexcept {
            return std::hash<DirectMusic::GUID>{}(s.getGuid());
        }
    };

    template<> struct hash<DirectMusic::StyleForm> {
        typedef DirectMusic::StyleForm argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const noexcept {
            return std::hash<DirectMusic::GUID>{}(s.getGuid());
        }
    };
}

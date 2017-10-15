#pragma once

#include "Forms.h"
#include "Structs.h"

namespace DirectMusic {
    typedef std::pair<DMUS_IO_BAND_ITEM_HEADER2, BandForm> BandItem;

    class BandTrack
        : public SubtrackForm {
    public:
        BandTrack(const DirectMusic::Riff::Chunk& chunk);
        const DMUS_IO_BAND_TRACK_HEADER& getHeader() const { return m_header; }
        const GUID& getGuid() const { return m_guid; }
        const DMUS_IO_VERSION& getVersion() const { return m_version; }
        const DirectMusic::Riff::Unfo& getInfo() const { return m_unfo; }
        const std::vector<BandItem>& getBands() const { return m_bands; }

    private:
        DMUS_IO_BAND_TRACK_HEADER m_header;
        GUID m_guid;
        DMUS_IO_VERSION m_version;
        DirectMusic::Riff::Unfo m_unfo;
        std::vector<BandItem> m_bands;
    };

    typedef std::pair<DMUS_IO_CHORD, std::vector<DMUS_IO_SUBCHORD>> ChordBody;
    class ChordTrack
        : public SubtrackForm {
    public:
        ChordTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::uint32_t getHeader() const { return m_header; }
        const std::vector<ChordBody>& getChords() { return m_chords;  }

    private:
        std::uint32_t m_header;
        std::vector<ChordBody> m_chords;
    };

    typedef std::pair<std::uint32_t, ReferenceList> Chordmap;

    class ChordmapTrack
        : public SubtrackForm {
    public:
        ChordmapTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<Chordmap>& getChordmaps() const { return m_chordmaps; }

    private:
        std::vector<Chordmap> m_chordmaps;
    };

    class CommandTrack
        : public SubtrackForm {
    public:
        CommandTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<DMUS_IO_COMMAND>& getCommands() const { return m_commands; }

    private:
        std::vector<DMUS_IO_COMMAND> m_commands;
    };

    typedef std::pair<DMUS_IO_LYRICSTRACK_EVENTHEADER, std::string> LyricsEvent;

    class LyricsTrack
        : public SubtrackForm {
    public:
        LyricsTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<LyricsEvent>& getLyrics() const { return m_lyrics; }

    private:
        std::vector<LyricsEvent> m_lyrics;
    };

    class MarkerTrack
        : public SubtrackForm {
    public:
        MarkerTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<DMUS_IO_VALID_START>& getValidStartPoints() const { return m_validStarts; }
        const std::vector<DMUS_IO_PLAY_MARKER>& getValidPlayPoints() const { return m_validPlays; }

    private:
        std::vector<DMUS_IO_VALID_START> m_validStarts;
        std::vector<DMUS_IO_PLAY_MARKER> m_validPlays;
    };

    class MuteTrack
        : public SubtrackForm {
    public:
        MuteTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<DMUS_IO_MUTE>& getData() const { return m_mutes; }

    private:
        std::vector<DMUS_IO_MUTE> m_mutes;
    };

    class ParameterControlTrack
        : public SubtrackForm {
    public:
        ParameterControlTrack(const DirectMusic::Riff::Chunk& chunk) {};
    };

    class PatternTrack
        : public SubtrackForm {
    public:
        PatternTrack(const DirectMusic::Riff::Chunk& chunk);
        const DMUS_IO_STYLE& getStyleHeader() const { return m_style; }
        const std::shared_ptr<Pattern>& getPattern() const { return m_pattern; }

    private:
        DMUS_IO_STYLE m_style;
        std::shared_ptr<Pattern> m_pattern;
    };

    class ScriptTrack
        : public SubtrackForm {
    public:
        ScriptTrack(const DirectMusic::Riff::Chunk& chunk) {};
    };

    class SegmentTriggerTrack
        : public SubtrackForm {
    public:
        SegmentTriggerTrack(const DirectMusic::Riff::Chunk& chunk) {};
    };

    class SequenceTrack
        : public SubtrackForm {
    public:
        SequenceTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<DMUS_IO_SEQ_ITEM>& getSequenceItems() const { return m_seqItems; }
        const std::vector<DMUS_IO_CURVE_ITEM>& getCurveItems() const { return m_curveItems; }

    private:
        std::vector<DMUS_IO_SEQ_ITEM> m_seqItems;
        std::vector<DMUS_IO_CURVE_ITEM> m_curveItems;
    };

    class SignpostTrack
        : public SubtrackForm {
    public:
        SignpostTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<DMUS_IO_SIGNPOST>& getSignposts() const { return m_signposts; }

    private:
        std::vector<DMUS_IO_SIGNPOST> m_signposts;
    };

    typedef std::pair<std::uint16_t, ReferenceList> StyleReference;

    class StyleTrack
        : public SubtrackForm {
    public:
        StyleTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<StyleReference> getStyles() const { return m_styles; }

    private:
        std::vector<StyleReference> m_styles;
    };

    class SysexTrack
        : public SubtrackForm {
    public:
        SysexTrack(const DirectMusic::Riff::Chunk& chunk) {};
    };

    class TempoTrack
        : public SubtrackForm {
    public:
        TempoTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<DMUS_IO_TEMPO_ITEM>& getItems() const { return m_items; }

    private:
        std::vector<DMUS_IO_TEMPO_ITEM> m_items;
    };

    class TimeSignatureTrack
        : public SubtrackForm {
    public:
        TimeSignatureTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<DMUS_IO_TIMESIGNATURE_ITEM>& getItems() const { return m_items; }

    private:
        std::vector<DMUS_IO_TIMESIGNATURE_ITEM> m_items;
    };

    class WaveTrack
        : public SubtrackForm {
    public:
        WaveTrack(const DirectMusic::Riff::Chunk& chunk) {};
    };
}
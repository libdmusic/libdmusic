#pragma once

#include "Forms.h"
#include "Structs.h"

namespace DirectMusic {
    typedef std::tuple<DMUS_IO_BAND_ITEM_HEADER2, BandForm> BandItem;

    class BandTrack
        : public SubtrackForm
    {
    public:
        BandTrack(const DirectMusic::Riff::Chunk& chunk);
        const DMUS_IO_BAND_TRACK_HEADER& getHeader() const { return m_header; }
        const GUID& getGuid() const { return m_guid; }
        const DMUS_IO_VERSION& getVersion() const { return m_version; }
        const DirectMusic::Riff::Unfo& getInfo() const { return m_unfo; }
        const std::uint32_t getPhysicalTime() const { return m_physTime; }
        const std::uint32_t getLogicalTime() const { return m_logicTime; }
        const std::vector<BandItem>& getBands() const { return m_bands; }

    private:
        DMUS_IO_BAND_TRACK_HEADER m_header;
        GUID m_guid;
        DMUS_IO_VERSION m_version;
        DirectMusic::Riff::Unfo m_unfo;
        std::uint32_t m_physTime, m_logicTime;
        std::vector<BandItem> m_bands;
    };

    class ChordTrack
        : public SubtrackForm
    {
    public:
        ChordTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::uint32_t getHeader() const { return m_header; }
        const DMUS_IO_CHORD& getChord() const { return m_chord; }
        const std::vector<DMUS_IO_SUBCHORD>& getSubchords() const { return m_subchords; }

    private:
        std::uint32_t m_header;
        DMUS_IO_CHORD m_chord;
        std::vector<DMUS_IO_SUBCHORD> m_subchords;
    };

    typedef std::tuple<std::uint32_t, ReferenceList> Chordmap;

    class ChordmapTrack
        : public SubtrackForm
    {
    public:
        ChordmapTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<Chordmap>& getChordmaps() const { return m_chordmaps; }

    private:
        std::vector<Chordmap> m_chordmaps;
    };

    class CommandTrack
        : public SubtrackForm
    {
    public:
        CommandTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<DMUS_IO_COMMAND>& getCommands() const { return m_commands; }

    private:
        std::vector<DMUS_IO_COMMAND> m_commands;
    };

    typedef std::tuple<DMUS_IO_LYRICSTRACK_EVENTHEADER, std::wstring> LyricsEvent;

    class LyricsTrack
        : public SubtrackForm
    {
    public:
        LyricsTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<LyricsEvent>& getLyrics() const { return m_lyrics; }

    private:
        std::vector<LyricsEvent> m_lyrics;
    };

    class MarkerTrack
        : public SubtrackForm
    {
    public:
        MarkerTrack(const DirectMusic::Riff::Chunk& chunk);
        const std::vector<DMUS_IO_VALID_START>& getValidStartPoints() const { return m_validStarts; }
        const std::vector<DMUS_IO_PLAY_MARKER>& getValidPlayPoints() const { return m_validPlays; }

    private:
        std::vector<DMUS_IO_VALID_START> m_validStarts;
        std::vector<DMUS_IO_PLAY_MARKER> m_validPlays;
    };

    class MuteTrack
        : public SubtrackForm
    {
    public:
        MuteTrack(const DirectMusic::Riff::Chunk& chunk);
        const DMUS_IO_MUTE& getData() const { return m_mute; }

    private:
        DMUS_IO_MUTE m_mute;
    };

    class ParameterControlTrack
        : public SubtrackForm {};

    class PatternTrack
        : public SubtrackForm {};

    class ScriptTrack
        : public SubtrackForm {};

    class SegmentTriggerTrack
        : public SubtrackForm {};

    class SequenceTrack
        : public SubtrackForm {};

    class SignpostTrack
        : public SubtrackForm {};

    class StyleTrack
        : public SubtrackForm {};

    class SysexTrack
        : public SubtrackForm {};

    class TempoTrack
        : public SubtrackForm {};

    class TimeSignatureTrack
        : public SubtrackForm {};

    class WaveTrack
        : public SubtrackForm {};
}
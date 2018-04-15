#pragma once
#include <cstdint>
#include <memory>
#include "Midi.h"
#include <dmusic/Structs.h>
#include <dmusic/dls/Instrument.h>
#include <dmusic/InstrumentPlayer.h>

namespace DirectMusic {
    class PlayingContext;
    enum class SegmentTiming;

    enum class MusicMessageType {
        TempoChange,
        BandChange,
        GrooveLevel,
        ChordMessage,
        NoteOn,
        NoteOff,
        SegmentEnd,
        PatternEnd,
        ControlChange
    };

    class MusicMessage {
    public:
        MusicMessage(std::uint32_t time)
            : m_messageTime(time) {};
        std::uint32_t getMessageTime() const { return m_messageTime; };

        virtual void Execute(PlayingContext& ctx) = 0;
        virtual int getPriority() { return 0; }
        virtual std::shared_ptr<MusicMessage> Clone(std::uint32_t newTime) = 0;
        virtual MusicMessageType getMessageType() const = 0;

    protected:
        std::uint32_t m_messageTime;

        void changeTempo(PlayingContext& ctx, double tempo);
        std::shared_ptr<InstrumentPlayer> createInstrument(PlayingContext& ctx,
            std::uint8_t bank_lo, std::uint8_t bank_hi, std::uint8_t patch,
            const GUID& bandGuid, DirectMusic::DLS::DownloadableSound& dls,
            float volume, float pan);
        std::shared_ptr<InstrumentPlayer> createGMInstrument(PlayingContext& ctx,
            std::uint8_t bank_lo, std::uint8_t bank_hi, std::uint8_t patch,
            float volume, float pan);
        void setInstrument(PlayingContext& ctx, std::uint32_t channel, std::shared_ptr<InstrumentPlayer> instr);
        void playPattern(PlayingContext& ctx);
        void setGrooveLevel(PlayingContext& ctx, std::uint8_t level);
        const std::map<std::uint32_t, std::shared_ptr<InstrumentPlayer>>& getChannels(PlayingContext& ctx);
        void changeChord(PlayingContext& ctx, std::uint32_t chord, const std::vector<DMUS_IO_SUBCHORD>& subchords);
        void enqueueNextSegment(PlayingContext& ctx);
        bool isNextSegmentAvailable(PlayingContext& ctx);
        SegmentTiming getNextSegmentTiming(PlayingContext& ctx);
    };

    struct MusicMessageComparer {
        bool operator()(const std::shared_ptr<MusicMessage>& lhs, const std::shared_ptr<MusicMessage>& rhs) const {
            std::uint32_t ltime = lhs->getMessageTime(), rtime = rhs->getMessageTime();
            return ltime == rtime ? lhs->getPriority() < rhs->getPriority() : ltime > rtime;
        }
    };
}
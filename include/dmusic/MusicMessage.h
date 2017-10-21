#pragma once
#include <cstdint>
#include <memory>
#include "Midi.h"
#include <dmusic/Structs.h>
#include <dmusic/dls/Instrument.h>
#include <dmusic/InstrumentPlayer.h>

namespace DirectMusic {
    class PlayingContext;
    class MusicMessage {
    public:
        MusicMessage(std::uint32_t time)
            : m_messageTime(time) {};
        std::uint32_t getMessageTime() const { return m_messageTime; };

        virtual void Execute(PlayingContext& ctx) = 0;

    protected:
        std::uint32_t m_messageTime;

        void changeTempo(PlayingContext& ctx, double tempo);
        std::shared_ptr<InstrumentPlayer> createInstrument(PlayingContext& ctx,
            std::uint8_t bank_lo, std::uint8_t bank_hi, std::uint8_t patch,
            const DirectMusic::DLS::DownloadableSound& dls,
            float volume, float pan);
        void setInstrument(PlayingContext& ctx, std::uint32_t channel, std::shared_ptr<InstrumentPlayer> instr);
        void setGrooveLevel(PlayingContext& ctx, std::uint8_t level);
        const std::map<std::uint32_t, std::shared_ptr<InstrumentPlayer>>& getChannels(PlayingContext& ctx);
        void changeChord(PlayingContext& ctx, std::uint32_t chord, const std::vector<DMUS_IO_SUBCHORD>& subchords);
    };

    struct MusicMessageComparer {
        bool operator()(const std::shared_ptr<MusicMessage>& lhs, const std::shared_ptr<MusicMessage>& rhs) const {
            return lhs->getMessageTime() > rhs->getMessageTime();
        }
    };
}
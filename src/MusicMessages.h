#pragma once

#include <map>
#include <locale>
#include <codecvt>
#include <dmusic/MusicMessage.h>
#include <dmusic/Tracks.h>

namespace DirectMusic {
    class TempoChangeMessage : public MusicMessage {
    public:
        TempoChangeMessage(std::uint32_t time, double tempo)
        : MusicMessage(time), 
        m_tempo(tempo) { }

        virtual void Execute(PlayingContext& ctx);

    private:
        double m_tempo;
    };
    
    class BandChangeMessage : public MusicMessage {
    public:
        BandChangeMessage(PlayingContext& ctx, std::uint32_t time, const DirectMusic::BandForm& form);

        virtual void Execute(PlayingContext& ctx);

    private:
        std::map<std::uint32_t, std::shared_ptr<InstrumentPlayer>> instruments;
    };

    class GrooveLevelMessage : public MusicMessage {
    public:
        GrooveLevelMessage(std::uint32_t time, std::uint8_t grooveLevel, std::uint8_t grooveRange)
            : MusicMessage(time),
            m_level(grooveLevel),
            // The specs say that if the range is odd, the actual range is to be considered as range - 1
            m_range(grooveRange % 2 == 0 ? grooveRange : grooveRange  - 1) {};

        virtual void Execute(PlayingContext& ctx);

    private:
        std::uint8_t m_level, m_range;
    };
}
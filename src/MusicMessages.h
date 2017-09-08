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
}
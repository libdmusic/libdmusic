#pragma once

#if !DMUSIC_DLS_PLAYER
#error "DlsPlayer was not enabled"
#endif

#include <cstdint>
#include <functional>
#include <unordered_map>
#include "dls/DownloadableSound.h"
#include "InstrumentPlayer.h"
#include "PlayingContext.h"

class TinySoundFont;

namespace DirectMusic {
    class DlsPlayer : public InstrumentPlayer {
    private:
        int m_preset;
        std::shared_ptr<TinySoundFont> m_soundfont;

        static std::unordered_map<DirectMusic::DLS::DownloadableSound, std::shared_ptr<TinySoundFont>> m_soundfonts;

        DlsPlayer(std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
            DirectMusic::DLS::DownloadableSound& dls,
            const GUID& bandId,
            std::uint32_t sampleRate,
            std::uint32_t channels,
            float volume,
            float pan);

    public:
        virtual std::uint32_t renderBlock(std::int16_t *buffer, std::uint32_t count, bool mix) noexcept;

        /// Instructs the synthesizer to start playing a note
        virtual void noteOn(std::uint8_t note, std::uint8_t velocity);

        /// Instructs the synthesizer to stop playing a note
        virtual void noteOff(std::uint8_t note, std::uint8_t velocity);

        virtual void allNotesOff();

        /// Sends a "channel pressure" message
        virtual void channelPressure(std::uint8_t val);

        /// Sends a "polyphonic aftertouch" message to a note
        virtual void polyAftertouch(std::uint8_t note, std::uint8_t val);

        /// Sends a "control change" message
        virtual void controlChange(DirectMusic::Midi::Control control, float val);

        /// Sends a "program change" (aka "patch change") message
        virtual void programChange(std::uint8_t program);

        /// Sends a "pitch bend" message
        virtual void pitchBend(std::int16_t val);

        static PlayerFactory createFactory();
        static GMPlayerFactory createGMFactory(DLS::DownloadableSound& dls);
    };
}
#pragma once

#if !DMUSIC_TSF_SUPPORT
#error "TinySoundFont rendering is not enabled"
#endif

#include <cstdint>
#include <functional>
#include "dls/DownloadableSound.h"
#include "InstrumentPlayer.h"
#include "PlayingContext.h"

struct tsf;

namespace DirectMusic {
    class SoundFontPlayer : public InstrumentPlayer {
    private:
        int m_preset;
        tsf* m_soundfont;

        SoundFontPlayer(tsf* soundfont,
            std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
            DirectMusic::DLS::DownloadableSound& dls,
            std::uint32_t sampleRate,
            std::uint32_t channels,
            float volume,
            float pan);
    public:

        ~SoundFontPlayer();

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

        static PlayerFactory createMultiFactory(const std::string soundFontDir);
        static PlayerFactory createFactory(const std::string& soundFont);
    };
}
#pragma once

#include <dmusic/InstrumentPlayer.h>

namespace DirectMusic {
    class DummyPlayer : public InstrumentPlayer {
    public:
        DummyPlayer(std::uint8_t bank_lo, std::uint8_t bank_hi, std::uint8_t patch,
            DirectMusic::DLS::DownloadableSound& dls,
            std::uint32_t sampleRate,
            std::uint32_t audioChannels,
            float volume,
            float pan) : InstrumentPlayer(bank_lo, bank_hi, patch, dls, sampleRate, audioChannels, volume, pan) {}

        virtual std::uint32_t renderBlock(std::int16_t *buffer, std::uint32_t count, bool mix) noexcept {
            return count;
        };

        /// Instructs the synthesizer to start playing a note
        virtual void noteOn(std::uint8_t note, std::uint8_t velocity) {};

        /// Instructs the synthesizer to stop playing a note
        virtual void noteOff(std::uint8_t note, std::uint8_t velocity) {};

        virtual void allNotesOff() {};

        /// Sends a "channel pressure" message
        virtual void channelPressure(std::uint8_t val) {};

        /// Sends a "polyphonic aftertouch" message to a note
        virtual void polyAftertouch(std::uint8_t note, std::uint8_t val) {};

        /// Sends a "control change" message
        virtual void controlChange(DirectMusic::Midi::Control control, float val) {};

        /// Sends a "program change" (aka "patch change") message
        virtual void programChange(std::uint8_t program) {};

        /// Sends a "pitch bend" message
        virtual void pitchBend(std::int16_t val) {};
    };
}
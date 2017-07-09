#pragma once

#include <cstdint>
#include "Midi.h"
#include "dls/Instrument.h"

namespace DirectMusic {
    /** \brief Interface for objects that can respond to MIDI data and render audio
     * This class is provided as a mean to abstract message passing from the
     * actual audio rendering.
     */
    class InstrumentPlayer {
    public:
        InstrumentPlayer(const DirectMusic::DLS::Instrument& instrument,
                        std::uint32_t sampleRate,
                        std::uint32_t audioChannels)
            : m_instrument(instrument) {};

        /// Renders the following `count` samples of audio
        /// WARNING: this method is very performance-sensitive; it is important
        /// that it does what it has to do in the least amount of time, and
        /// it must NOT throw.
        virtual std::uint32_t renderBlock(std::int16_t *buffer, std::uint32_t count) noexcept = 0;

        /// Instructs the synthesizer to start playing a note
        virtual bool noteOn(std::uint8_t note, std::uint8_t velocity) = 0;

        /// Instructs the synthesizer to stop playing a note
        virtual void noteOff(std::uint8_t note, std::uint8_t velocity) = 0;

        /// Sends a "channel pressure" message
        virtual void channelPressure(std::uint8_t val) = 0;

        /// Sends a "polyphonic aftertouch" message to a note
        virtual void polyAftertouch(std::uint8_t note, std::uint8_t val) = 0;

        /// Sends a "control change" message
        virtual void controlChange(DirectMusic::Midi::Control control, std::int32_t val) = 0;

        /// Sends a "program change" (aka "patch change") message
        virtual void programChange(std::uint8_t program) = 0;

        /// Sends a "pitch bend" message
        virtual void pitchBend(std::int16_t val) = 0;

    protected:
        const DirectMusic::DLS::Instrument& m_instrument;
    };
}

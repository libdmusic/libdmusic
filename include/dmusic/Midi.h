#pragma once

#include <cstdint>

namespace DirectMusic {
    namespace Midi {
        enum class Control : std::uint8_t {
            BankSelect = 0x00,
            ModWheel = 0x01,
            BreathCtl = 0x02,
            FootCtl = 0x04,
            Portamento = 0x05,
            DataMsb = 0x06,
            ChannelVolume = 0x07,
            Balance = 0x08,
            Pan = 0x0A,
            ExpressionCtl = 0x0B,
            Effect1Ctl = 0x0C,
            Effect2Ctl = 0x0D,
            GeneralPurpose1 = 0x10,
            GeneralPurpose2 = 0x11,
            GeneralPurpose3 = 0x12,
            GeneralPurpose4 = 0x13
        };

        enum class Message : std::uint8_t {
            NoteOff = 0x08,
            NoteOn = 0x09,
            PolyAfterouch = 0x0A,
            ControlChange = 0x0B,
            ProgramChange = 0x0C,
            ChannelPressure = 0x0D,
            PitchBend = 0x0E
        };

        struct Event {
            Message Type : 4;
            std::uint8_t Channel : 4;
            union {
                struct {
                    std::uint8_t NoteNumber;
                    std::uint8_t Velocity;
                } NoteEvent;

                struct {
                    std::uint8_t NoteNumber;
                    std::uint8_t Pressure;
                } PolyAftertouch;

                struct {
                    Control ControlType;
                    std::uint8_t Data;
                } ControlChange;

                struct {
                    std::uint8_t Pressure;
                } ChannelPressure;

                struct {
                    std::uint8_t Program;
                } ProgramChange;

                struct {
                    std::uint16_t Pitch;
                } PitchWheel;
            };
        };
    }
}

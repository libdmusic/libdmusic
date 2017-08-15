#pragma once
#include <cstdint>
#include <memory>
#include "Midi.h"

namespace DirectMusic {
    enum class MessageType {
        Tempo,
        TimeSignature,
        NoteOn,
        NoteOff,
        ChannelPressure,
        PolyAftertouch,
        ControlChange,
        ProgramChange,
        PitchBend
    };

    class MusicMessage {
    public:
        MusicMessage(std::uint32_t time)
            : m_messageTime(time) {};

        virtual MessageType getMessageType() const = 0;
        std::uint32_t getMessageTime() const { return m_messageTime; };

    protected:
        std::uint32_t m_messageTime;
    };
    
    struct MusicMessageComparer {
        bool operator()(const std::shared_ptr<MusicMessage>& lhs, const std::shared_ptr<MusicMessage>& rhs) const {
            return lhs->getMessageTime() > rhs->getMessageTime();
        }
    };

    class NoteOnMessage : public MusicMessage {
    public:
        NoteOnMessage(std::uint32_t time, std::uint8_t note, std::uint8_t velocity, std::uint8_t channel)
            : MusicMessage(time),
            m_note(note),
            m_velocity(velocity),
            m_channel(channel) {};

        std::uint8_t getNote() const { return m_note; }
        std::uint8_t getVelocity() const { return m_velocity; }
        std::uint8_t getChannel() const { return m_channel; }

        virtual MessageType getMessageType() {
            return MessageType::NoteOn;
        }

    private:
        std::uint8_t m_note, m_velocity, m_channel;
    };

    class NoteOffMessage : public MusicMessage {
    public:
        NoteOffMessage(std::uint32_t time, std::uint8_t note, std::uint8_t velocity, std::uint8_t channel)
            : MusicMessage(time),
            m_note(note),
            m_velocity(velocity),
            m_channel(channel) {};

        std::uint8_t getNote() const { return m_note; }
        std::uint8_t getVelocity() const { return m_velocity; }

        virtual MessageType getMessageType() {
            return MessageType::NoteOff;
        }

    private:
        std::uint8_t m_note, m_velocity, m_channel;
    };

    class ChannelPressureMessage : public MusicMessage {
    public:
        ChannelPressureMessage(std::uint32_t time, std::uint8_t value, std::uint8_t channel)
            : MusicMessage(time),
            m_value(value),
            m_channel(channel) {};

        std::uint8_t getValue() const { return m_value; }

        virtual MessageType getMessageType() {
            return MessageType::ChannelPressure;
        }

    private:
        std::uint8_t m_value, m_channel;
    };

    class PolyAftertouchMessage : public MusicMessage {
    public:
        PolyAftertouchMessage(std::uint32_t time, std::uint8_t note, std::uint8_t value, std::uint8_t channel)
            : MusicMessage(time),
            m_note(note),
            m_value(value),
            m_channel(channel) {};

        std::uint8_t getNote() const { return m_note; }
        std::uint8_t getValue() const { return m_value; }

        virtual MessageType getMessageType() {
            return MessageType::PolyAftertouch;
        }

    private:
        std::uint8_t m_value, m_note, m_channel;
    };

    class ControlChangeMessage : public MusicMessage {
    public:
        ControlChangeMessage(std::uint32_t time, DirectMusic::Midi::Control control, std::int32_t value, std::uint8_t channel)
            : MusicMessage(time),
            m_control(control),
            m_value(value),
            m_channel(channel) {};

        DirectMusic::Midi::Control getControl() const { return m_control; }
        std::uint8_t getValue() const { return m_value; }

        virtual MessageType getMessageType() {
            return MessageType::ControlChange;
        }

    private:
        DirectMusic::Midi::Control m_control;
        std::int32_t m_value;
        std::uint8_t m_channel;
    };

    class ProgramChangeMessage : public MusicMessage {
    public:
        ProgramChangeMessage(std::uint32_t time, std::uint8_t program, std::uint8_t channel)
            : MusicMessage(time),
            m_program(program),
            m_channel(channel) {};

        std::uint8_t getProgram() const { return m_program; }

        virtual MessageType getMessageType() {
            return MessageType::ProgramChange;
        }

    private:
        std::uint8_t m_program, m_channel;
    };

    class PitchBendMessage : public MusicMessage {
    public:
        PitchBendMessage(std::uint32_t time, std::int16_t value, std::uint8_t channel)
            : MusicMessage(time),
            m_value(value),
            m_channel(channel) {};

        std::int16_t getValue() const { return m_value; }

        virtual MessageType getMessageType() {
            return MessageType::PitchBend;
        }

    private:
        std::int16_t m_value;
        std::uint8_t m_channel;
    };
}
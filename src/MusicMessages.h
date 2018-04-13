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

        virtual std::shared_ptr<MusicMessage> Clone(std::uint32_t newTime) {
            return std::make_shared<TempoChangeMessage>(newTime, m_tempo);
        }

        virtual MusicMessageType getMessageType() const { return MusicMessageType::TempoChange; }

        virtual void Execute(PlayingContext& ctx);

    private:
        double m_tempo;
    };
    
    class BandChangeMessage : public MusicMessage {
    public:
        BandChangeMessage(std::uint32_t time, std::map<std::uint32_t, std::shared_ptr<InstrumentPlayer>> instr)
            : MusicMessage(time)
            , instruments(instr) {}
        BandChangeMessage(PlayingContext& ctx, std::uint32_t time, const DirectMusic::BandForm& form);

        virtual std::shared_ptr<MusicMessage> Clone(std::uint32_t newTime) {
            return std::make_shared<BandChangeMessage>(newTime, instruments);
        }

        virtual MusicMessageType getMessageType() const { return MusicMessageType::BandChange; }

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

        virtual std::shared_ptr<MusicMessage> Clone(std::uint32_t newTime) {
            return std::make_shared<GrooveLevelMessage>(newTime, m_level, m_range);
        }

        virtual MusicMessageType getMessageType() const { return MusicMessageType::GrooveLevel; }

        virtual void Execute(PlayingContext& ctx);
        virtual int getPriority() { return -1; };

    private:
        std::uint8_t m_level, m_range;
    };

    class ChordMessage : public MusicMessage {
    public:
        ChordMessage(std::uint32_t time, std::uint32_t chord, const std::vector<DMUS_IO_SUBCHORD> subchords)
            : MusicMessage(time),
            m_chord(chord),
            m_subchords(subchords) {};

        virtual std::shared_ptr<MusicMessage> Clone(std::uint32_t newTime) {
            return std::make_shared<ChordMessage>(newTime, m_chord, m_subchords);
        }

        virtual MusicMessageType getMessageType() const { return MusicMessageType::ChordMessage; }

        virtual void Execute(PlayingContext& ctx);
        virtual int getPriority() { return 1; };

    private:
        std::uint32_t m_chord;
        const std::vector<DMUS_IO_SUBCHORD> m_subchords;
    };

    class NoteOnMessage : public MusicMessage {
    public:
        NoteOnMessage(std::uint32_t time, std::uint8_t note, std::uint8_t velocity, std::uint8_t velRange, std::uint32_t channel, std::uint32_t channelAlt)
            : MusicMessage(time),
            m_note(note),
            m_vel(velocity),
            m_velRange(velRange),
            m_channel(channel),
            m_channelAlt(channelAlt) {}

        virtual std::shared_ptr<MusicMessage> Clone(std::uint32_t newTime) {
            return std::make_shared<NoteOnMessage>(newTime, m_note, m_vel, m_velRange, m_channel, m_channelAlt);
        }

        virtual MusicMessageType getMessageType() const { return MusicMessageType::NoteOn; }

        virtual void Execute(PlayingContext& ctx);

    private:
        std::uint8_t m_note, m_vel, m_velRange;
        std::uint32_t m_channel, m_channelAlt;
    };

    class NoteOffMessage : public MusicMessage {
    public:
        NoteOffMessage(std::uint32_t time, std::uint8_t note, std::uint32_t channel, std::uint32_t channelAlt)
            : MusicMessage(time),
            m_note(note),
            m_channel(channel),
            m_channelAlt(channelAlt) {}

        virtual std::shared_ptr<MusicMessage> Clone(std::uint32_t newTime) {
            return std::make_shared<NoteOffMessage>(newTime, m_note, m_channel, m_channelAlt);
        }

        virtual MusicMessageType getMessageType() const { return MusicMessageType::NoteOff; }

        virtual void Execute(PlayingContext& ctx);

    private:
        std::uint8_t m_note;
        std::uint32_t m_channel, m_channelAlt;
    };

    class SegmentEndMessage : public MusicMessage {
    public:
        SegmentEndMessage(std::uint32_t time)
            : MusicMessage(time) {}

        virtual std::shared_ptr<MusicMessage> Clone(std::uint32_t newTime) {
            return std::make_shared<SegmentEndMessage>(newTime);
        }

        virtual MusicMessageType getMessageType() const { return MusicMessageType::SegmentEnd; }

        virtual void Execute(PlayingContext& ctx);
    };

    class PatternEndMessage : public MusicMessage {
    public:
        PatternEndMessage(std::uint32_t time)
            : MusicMessage(time) {}

        virtual std::shared_ptr<MusicMessage> Clone(std::uint32_t newTime) {
            return std::make_shared<PatternEndMessage>(newTime);
        }

        virtual MusicMessageType getMessageType() const { return MusicMessageType::PatternEnd; }

        virtual int getPriority() { return -2; }

        virtual void Execute(PlayingContext& ctx);
    };

    class ControlChangeMessage : public MusicMessage {
    public:
        ControlChangeMessage(std::uint32_t time, std::uint32_t channel, std::uint32_t channelAlt, DirectMusic::Midi::Control control, float value)
            : MusicMessage(time),
            m_control(control),
            m_channel(channel),
            m_channelAlt(channelAlt),
            m_value(value) {}

        virtual std::shared_ptr<MusicMessage> Clone(std::uint32_t newTime) {
            return std::make_shared<ControlChangeMessage>(newTime, m_channel, m_channelAlt, m_control, m_value);
        }

        virtual MusicMessageType getMessageType() const { return MusicMessageType::ControlChange; }

        virtual void Execute(PlayingContext& ctx);

    private:
        float m_value;
        std::uint32_t m_channel, m_channelAlt;
        DirectMusic::Midi::Control m_control;
    };
}
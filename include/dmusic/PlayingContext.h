#pragma once

#include <cstdint>
#include <memory>
#include <map>
#include <queue>
#include <mutex>
#include "Structs.h"
#include "InstrumentPlayer.h"
#include "Enums.h"
#include "Forms.h"
#include "Loader.h"
#include "dls/DownloadableSound.h"
#include "MusicMessage.h"

namespace DirectMusic {
    using PlayerFactory = std::function<std::shared_ptr<InstrumentPlayer>(
        std::uint8_t, std::uint8_t, std::uint8_t,
        const DirectMusic::DLS::DownloadableSound&,
        std::uint32_t,
        std::uint32_t)>;

    using MessageQueue = std::priority_queue<std::shared_ptr<MusicMessage>, std::vector<std::shared_ptr<MusicMessage>>, MusicMessageComparer>;

    /// This the main interface to the DirectMusic emulation layer
    class PlayingContext {
        friend class MusicMessage;
    private:
        struct Pattern {
            std::uint8_t grooveLower;
            std::uint8_t grooveUpper;
            DMUS_IO_TIMESIG timeSignature;
            std::vector<DMUS_IO_STYLENOTE> notes;
            std::uint32_t performanceChannel;
        };

        struct Segment {
            std::vector<std::shared_ptr<MusicMessage>> messages;
            bool infiniteLoop;
            std::uint32_t numLoops;
            std::vector<Pattern> patterns;

            Pattern* getRandomPattern(std::uint8_t grooveLevel) const;
        };

        PlayerFactory m_instrumentFactory;
        std::uint32_t m_sampleRate, m_audioChannels;
        Loader& m_loader;
        std::map<std::uint32_t, std::shared_ptr<InstrumentPlayer>> m_performanceChannels;
        double m_musicTime;
        double m_tempo;
        std::uint8_t m_grooveLevel;
        std::uint32_t m_chord;
        DMUS_IO_TIMESIG m_signature;
        std::mutex m_queueMutex;
        MessageQueue m_messageQueue;
        std::unique_ptr<Segment> m_primarySegment = nullptr;

        template<typename T>
        static std::shared_ptr<T> genObjFromChunkData(const std::vector<std::uint8_t>& data) {
            if (data.empty()) return nullptr;
            DirectMusic::Riff::Chunk c(data.data());
            return std::make_shared<T>(c);
        }

    public:
        static const std::uint32_t PulsesPerQuarterNote = 768;

        /// Creates a new playing contest with the specified sampling rate and
        /// number of audio channels (normally 1 (mono) or 2 (stereo))
        PlayingContext(std::uint32_t sampleRate,
            std::uint32_t audioChannels,
            PlayerFactory instrumentFactory)
            : m_sampleRate(sampleRate),
            m_audioChannels(audioChannels),
            m_instrumentFactory(instrumentFactory),
            m_loader(*(new Loader())),
            m_musicTime(0.0),
            m_grooveLevel(0) {}

        /// Renders the following audio block
        void renderBlock(std::int16_t *data, std::uint32_t count, float volume = 1) noexcept;

        /// Begins the playback of a segment
        void playSegment(const SegmentForm& segment/*, DMUS_SEGF_FLAGS flags, std::int64_t startTime = 0*/);
        /*
        void playTransition(const SegmentForm& segment,
                            DMUS_COMMANDT_TYPES command,
                            DMUS_COMPOSEF_FLAGS flags,
                            std::shared_ptr<ChordmapForm> chordmap = nullptr);*/

        /// Overrides the default loader with a custom one
        void provideLoader(const Loader& l) { m_loader = l; };

        /// Loads a segment file
        std::shared_ptr<SegmentForm> loadSegment(const std::string& file) const {
            std::vector<std::uint8_t> data = m_loader.loadFile(file);
            return genObjFromChunkData<SegmentForm>(data);
        }

        /// Loads a style file
        std::shared_ptr<StyleForm> loadStyle(const std::string& file) const {
            std::vector<std::uint8_t> data = m_loader.loadFile(file);
            return genObjFromChunkData<StyleForm>(data);
        }

        /// Loads an instrument collection
        std::shared_ptr<DirectMusic::DLS::DownloadableSound> loadInstrumentCollection(const std::string& file) const {
            std::vector<std::uint8_t> data = m_loader.loadFile(file);
            return genObjFromChunkData<DirectMusic::DLS::DownloadableSound>(data);
        }

        double getTime() const { return m_musicTime; }
    };
}

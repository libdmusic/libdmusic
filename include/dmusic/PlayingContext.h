#pragma once

#include <cstdint>
#include <memory>
#include <map>
#include <queue>
#include <mutex>
#include "Common.h"
#include "Structs.h"
#include "InstrumentPlayer.h"
#include "Enums.h"
#include "Forms.h"
#include "Loader.h"
#include "dls/DownloadableSound.h"
#include "MusicMessage.h"

namespace DirectMusic {
    /// This the main interface to the DirectMusic emulation layer
    template<typename T>
    class PlayingContext {
    private:
        struct TimeSignature {
            std::uint8_t beatsPerMeasure;
            std::uint8_t beat;
            std::uint16_t subdivisions;
        };

        std::uint32_t m_sampleRate, m_audioChannels;
        const Loader& m_loader;
        std::map<int, std::shared_ptr<InstrumentPlayer>> m_performanceChannels;
        std::uint32_t m_musicTime;
        double m_tempo;
        TimeSignature m_signature;
        std::mutex m_mutex;
        std::priority_queue<std::shared_ptr<MusicMessage>, std::vector<std::shared_ptr<MusicMessage>>, MusicMessageComparer> m_messageQueue;

        template<typename T1>
        static std::shared_ptr<T1> genObjFromChunkData(const std::vector<std::uint8_t>& data) {
            if (data.empty()) return nullptr;
            DirectMusic::Riff::Chunk c(data.data());
            return std::make_shared<T1>(c);
        }

        void setTempo(double tempo) {
            m_mutex.lock();
            m_tempo = tempo;
            m_mutex.unlock();
        }

        void setTimeSignature(std::uint8_t beatsPerMeasure, std::uint8_t beat, std::uint16_t subdivisions) {
            m_mutex.lock();
            m_signature = TimeSignature();
            m_signature.beatsPerMeasure = beatsPerMeasure;
            m_signature.beat = beat;
            m_signature.subdivisions = subdivisions;
            m_mutex.unlock();
        }

    public:
        static const std::uint32_t PulsesPerQuarterNote = 768;

        /// Creates a new playing contest with the specified sampling rate and
        /// number of audio channels (normally 1 (mono) or 2 (stereo))
        PlayingContext(std::uint32_t sampleRate, std::uint32_t audioChannels)
            : m_sampleRate(sampleRate),
            m_audioChannels(audioChannels),
            m_loader(Loader()),
            m_musicTime(0) {}

        /// Renders the following audio block
        void renderBlock(std::int16_t *data, std::uint32_t count, float volume = 1) noexcept;

        /// Begins the playback of a segment
        void playSegment(const SegmentForm& segment, DMUS_SEGF_FLAGS flags, std::int64_t startTime = 0);

        void playTransition(const SegmentForm& segment,
                            DMUS_COMMANDT_TYPES command,
                            DMUS_COMPOSEF_FLAGS flags,
                            std::shared_ptr<ChordmapForm> chordmap = nullptr);

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

        std::uint32_t getTime() const { return m_musicTime; }
    };
}

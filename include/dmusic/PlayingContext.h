#pragma once

#include <cstdint>
#include <memory>
#include <map>
#include <unordered_map>
#include <queue>
#include <mutex>
#include <functional>
#include <utility>
#include <fstream>
#include "Common.h"
#include "Structs.h"
#include "InstrumentPlayer.h"
#include "Enums.h"
#include "Forms.h"
#include "dls/DownloadableSound.h"
#include "MusicMessage.h"

namespace DirectMusic {
    using PlayerFactory = std::function<std::shared_ptr<InstrumentPlayer>(
        std::uint8_t, std::uint8_t, std::uint8_t, // Bank lo, Bank hi, patch
        const GUID& bandId,
        DirectMusic::DLS::DownloadableSound&,
        std::uint32_t, // Sample rate
        std::uint32_t, // Channels
        float, // Volume
        float)>; // Pan

    using GMPlayerFactory = std::function<std::shared_ptr<InstrumentPlayer>(
        std::uint8_t, std::uint8_t, std::uint8_t, // Bank lo, Bank hi, patch
        std::uint32_t, // Sample rate
        std::uint32_t, // Channels
        float, // Volume
        float)>; // Pan

    using MessageQueue = std::priority_queue<std::shared_ptr<MusicMessage>, std::vector<std::shared_ptr<MusicMessage>>, MusicMessageComparer>;
    using GuidStringPair = std::pair<GUID, std::string>;

    class SegmentInfo;

    enum class SegmentTiming {
        Grid,     //< Aligns the segment to play at a grid boundary
        Beat,     //< Aligns the segment to play at a beat boundary
        Measure,  //< Aligns the segment to play at a measure boundary
        Pattern,  //< Aligns the segment to play at a pattern boundary
        Immediate //< Plays the segment immediately
    };

    /// This the main interface to the DirectMusic emulation layer
    class PlayingContext {
        friend class MusicMessage;
        friend class SegmentInfo;

    private:
        struct Pattern {
            DMUS_IO_PATTERN header;
            std::vector<std::pair<DMUS_IO_PARTREF, StylePart>> parts;
        };

        bool getRandomPattern(const SegmentInfo& segm, std::uint8_t grooveLevel, Pattern* output) const;

        PlayerFactory m_instrumentFactory;
        GMPlayerFactory  m_gminstrumentFactory; //< Used to instantiate instruments that come from GM patches
        std::uint32_t m_sampleRate, m_audioChannels;
        std::function<std::vector<std::uint8_t>(const std::string&)> m_loader;
        std::map<std::uint32_t, std::shared_ptr<InstrumentPlayer>> m_performanceChannels;
        std::uint32_t m_musicTime;
        double m_tempo;
        std::uint8_t m_grooveLevel;
        std::uint32_t m_chord;
        std::vector<DMUS_IO_SUBCHORD> m_subchords;
        DMUS_IO_TIMESIG m_signature;
        std::mutex m_queueMutex;
        MessageQueue m_messageQueue, m_patternMessageQueue;
        std::shared_ptr<SegmentInfo> m_primarySegment = nullptr, m_nextSegment = nullptr;
        std::uint32_t m_currentSegmentStart;
        SegmentTiming m_nextSegmentTiming;

        std::map<GUID, std::shared_ptr<DirectMusic::DLS::DownloadableSound>> m_bands;
        std::unordered_map<GuidStringPair, std::shared_ptr<StyleForm>> m_styles;

        template<typename T>
        static std::shared_ptr<T> genObjFromChunkData(const std::vector<std::uint8_t>& data) {
            if (data.empty()) return nullptr;
            DirectMusic::Riff::Chunk c(data.data());
            return std::make_shared<T>(c);
        }

        void enqueueSegment(const std::shared_ptr<SegmentInfo>& segment);

        void renderAudio(std::int16_t *data, std::uint32_t count, float volume) noexcept;

    public:

        static const std::uint32_t PulsesPerQuarterNote = 768;

        /// Creates a new playing context with the specified sampling rate and
        /// number of audio channels (normally 1 (mono) or 2 (stereo))
        PlayingContext(std::uint32_t sampleRate,
            std::uint32_t audioChannels,
            PlayerFactory instrumentFactory,
            GMPlayerFactory gminstrumentFactory = nullptr)
            : m_sampleRate(sampleRate),
            m_audioChannels(audioChannels),
            m_instrumentFactory(instrumentFactory),
            m_gminstrumentFactory(gminstrumentFactory),
            m_musicTime(0),
            m_grooveLevel(1),
            m_tempo(100),
            m_primarySegment(nullptr)
        {
            m_loader = [](const std::string& file) -> std::vector<std::uint8_t> {
                std::ifstream inputStream(file, std::ios::binary | std::ios::ate);
                if (!inputStream.is_open()) {
                    return std::vector<std::uint8_t>();
                }
                std::vector<std::uint8_t> buffer(inputStream.tellg());
                inputStream.seekg(0);
                inputStream.read((char*)buffer.data(), buffer.size());
                inputStream.close();
                return buffer;
            };

            m_signature.bBeat = 4;
            m_signature.bBeatsPerMeasure = 4;
            m_signature.wGridsPerBeat = 4;
        }

        /// Renders the following audio block
        void renderBlock(std::int16_t *data, std::uint32_t count, float volume = 1) noexcept;

        /// Prepares a segment for being played
        std::shared_ptr<SegmentInfo> prepareSegment(const SegmentForm& segment);

        /// Begins the playback of a segment
        void playSegment(const SegmentForm& segment, SegmentTiming timing = SegmentTiming::Immediate);
        void playSegment(std::shared_ptr<SegmentInfo> segment, SegmentTiming timing = SegmentTiming::Immediate);
        /*
        void playTransition(const SegmentForm& segment,
                            DMUS_COMMANDT_TYPES command,
                            DMUS_COMPOSEF_FLAGS flags,
                            std::shared_ptr<ChordmapForm> chordmap = nullptr);*/

        /// Overrides the default loader with a custom one
        void provideLoader(std::function<std::vector<std::uint8_t>(const std::string&)> l) { m_loader = l; };

        /// Loads a segment file
        std::shared_ptr<SegmentForm> loadSegment(const std::string& file) const {
            std::vector<std::uint8_t> data = m_loader(file);
            return genObjFromChunkData<SegmentForm>(data);
        }

        /// Loads a style file
        std::shared_ptr<StyleForm> loadStyle(const GUID& guid, const std::string& file);

        /// Loads an instrument collection
        std::shared_ptr<DirectMusic::DLS::DownloadableSound> loadInstrumentCollection(const GUID& guid, const GUID& bandGuid, const std::string& file);

        double getTime() const { return m_musicTime; }

        int getSampleRate() const { return m_sampleRate; }
        int getAudioChannels() const { return m_audioChannels; }
    };

    class SegmentInfo {
        friend class PlayingContext;

    public:
        inline bool operator ==(const SegmentInfo& b) const {
            return guid == b.guid && unfo == b.unfo;
        }

        inline bool operator !=(const SegmentInfo& b) const {
            return !(*this == b);
        }

    private:
        bool infiniteLoop;
        std::uint32_t numLoops;
        std::vector<PlayingContext::Pattern> patterns;
        std::vector<std::shared_ptr<MusicMessage>> messages;
        double initialTempo;
        DMUS_IO_TIMESIG initialSignature;
        std::uint32_t length;
        GUID guid;
        Riff::Unfo unfo;
    };
}

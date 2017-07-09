#pragma once

#include <cstdint>
#include <memory>
#include <map>
#include "Common.h"
#include "Structs.h"
#include "InstrumentPlayer.h"
#include "Enums.h"
#include "Forms.h"
#include "Loader.h"
#include "dls/DownloadableSound.h"

namespace DirectMusic {
    /// This the main interface to the DirectMusic emulation layer
    template<typename T>
    class PlayingContext {
    public:
        /// Creates a new playing contest with the specified sampling rate and
        /// number of audio channels (normally 1 (mono) or 2 (stereo))
        PlayingContext(std::uint32_t sampleRate, std::uint32_t audioChannels);

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
        std::shared_ptr<SegmentForm> loadSegment(const std::string& file) const;

        /// Loads a style file
        std::shared_ptr<StyleForm> loadStyle(const std::string& file) const;

        /// Loads an instrument collection
        std::shared_ptr<DirectMusic::DLS::DownloadableSound> loadInstrumentCollection(const std::string& file) const;

    private:
        std::uint32_t m_sampleRate, m_audioChannels;
        const Loader& m_loader;
        std::map<int, InstrumentPlayer> m_performanceChannels;
    };
}

#pragma once

#include "DlsCommon.h"
#include "../Common.h"
#include "../Riff.h"
#include <vector>
#include <string>
#include <map>

namespace DirectMusic {
    namespace DLS {
        /** \brief An Articulator modifies the playback of a wave file in a Downloadable Instrument
         *
         * See Downloadable Sounds Level 1, Version 1.1b - Chapter 1 p. ARTICULATION MODULES AND CONNECTIONS
         */
        class Articulator {
        public:
            Articulator(const DirectMusic::Riff::Chunk& c);
            const std::vector<ConnectionBlock>& getConnectionBlocks() const { return m_connectionBlocks; }

        private:
            std::vector<ConnectionBlock> m_connectionBlocks;
        };

        /// A Region specifies a continuous section of notes which refer to the same sample
        class Region {
        public:
            Region(const DirectMusic::Riff::Chunk& c);
            Region() {};
            const RegionHeader& getRegionHeader() const { return m_rgnHeader; }
            const WaveLink& getWaveLink() const { return m_waveLink; }
            const Wavesample& getWavesample() const { return m_wavesample; }
            const std::vector<Articulator> getArticulators() const { return m_articulators; }
            const std::vector<WavesampleLoop> getWavesampleLoops() const { return m_loops; }
        private:
            RegionHeader m_rgnHeader;
            WaveLink m_waveLink;
            Wavesample m_wavesample;
            std::vector<Articulator> m_articulators;
            std::vector<WavesampleLoop> m_loops;
        };

        /// An instrument is a collection of samples and articulators organized in regions
        class Instrument {
        public:
            Instrument(const DirectMusic::Riff::Chunk& c);
            const std::vector<Region>& getRegions() const { return m_regions; }
            const std::vector<Articulator>& getArticulators() const { return m_articulators; }
            const std::uint32_t getMidiBank() const { return m_midiBank; }
            const std::uint32_t getMidiProgram() const { return m_midiProgram; }
            const DirectMusic::Riff::Info& getInfo() const { return m_info; }
            const DirectMusic::GUID& getGuid() const { return m_dlsid; }

        private:
            DirectMusic::GUID m_dlsid;
            std::uint32_t m_midiBank, m_midiProgram;
            std::vector<Region> m_regions;
            std::vector<Articulator> m_articulators;
            DirectMusic::Riff::Info m_info;
        };
    }
}

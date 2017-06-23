#include "Common.h"
#include "../Guid.h"
#include "../Riff.h"
#include <vector>
#include <string>
#include <map>

namespace DirectMusic {
    namespace DLS {
        class Articulator {
        public:
            Articulator(DirectMusic::Riff::Chunk& c);
            const std::vector<ConnectionBlock>& getConnectionBlocks() const;

        private:
            std::vector<ConnectionBlock> m_connectionBlocks;
        };

        class Region {
        public:
            Region(DirectMusic::Riff::Chunk& c);
            const RegionHeader& getRegionHeader() const;
            const WaveLink& getWaveLink() const;
            const Wavesample& getWavesample() const;
            const std::vector<Articulator> getArticulators() const;
            const std::vector<WavesampleLoop> getWavesampleLoops() const;
        private:
            RegionHeader m_rgnHeader;
            WaveLink m_waveLink;
            Wavesample m_wavesample;
            std::vector<Articulator> m_articulators;
            std::vector<WavesampleLoop> m_loops;
        };

        class Instrument {
        public:
            Instrument(DirectMusic::Riff::Chunk& c);
            const std::vector<Region>& getRegions() const;
            const std::vector<Articulator>& getArticulators() const;
            const std::uint32_t getMidiBank() const;
            const std::uint32_t getMidiProgram() const;
            const DirectMusic::Riff::Info& getInfo() const;
            const DirectMusic::GUID& getGuid() const;

        private:
            DirectMusic::GUID m_dlsid;
            std::uint32_t m_midiBank, m_midiProgram;
            std::vector<Region> m_regions;
            std::vector<Articulator> m_articulators;
            DirectMusic::Riff::Info m_info;
        };
    }
}

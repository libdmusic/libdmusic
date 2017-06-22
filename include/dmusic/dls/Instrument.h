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
            const RGNRANGE getKeyRange() const;
            const RGNRANGE getVelocityRange() const;
            const std::uint16_t getSynthesisFlags() const;
            const std::uint16_t getKeyGroup() const;
            const std::vector<Articulator> getArticulators() const;
        private:
            RGNRANGE m_rangeKey, m_rangeVelocity;
            std::uint16_t m_fusOptions, m_usKeyGroup;
            std::vector<Articulator> m_articulators;
        };

        class Instrument {
        public:
            Instrument(DirectMusic::Riff::Chunk& c);
            const std::vector<Region>& getRegions() const;
            const std::vector<Articulator>& getArticulators() const;
            const Info& getInfo() const;
            const Guid& getGuid() const;

        private:
            Guid m_dlsid;
            std::uint64_t m_midiBank, m_midiProgram;
            std::vector<Region> m_regions;
            std::vector<Articulator> m_articulators;
            Info m_info;
        };
    }
}

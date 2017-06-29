#pragma once

#include "DlsCommon.h"
#include "Instrument.h"
#include "../Common.h"
#include "../Riff.h"
#include <cstdint>
#include <vector>
#include <ostream>

namespace DirectMusic {
    namespace DLS {
        class Wave {
        public:
            Wave(DirectMusic::Riff::Chunk& c);
            const GUID& getGuid() const;
            const DirectMusic::Riff::Info& getInfo() const;
            const WaveFormatEx& getWaveformat() const;
            const Wavesample& getWavesample() const;
            const std::vector<uint8_t>& getWavedata() const;
            const std::vector<WavesampleLoop>& getWavesampleLoops() const;

            /// Outputs the contents of the sample into a readable WAV container file
            void writeToStream(std::ostream& stream) const;

            /// Returns the contents of the sample as a WAV container
            std::vector<std::uint8_t> getWaveFile() const;

        private:
            GUID m_dlsid;
            DirectMusic::Riff::Info m_info;
            WaveFormatEx m_fmtex;
            std::vector<std::uint8_t> m_extraFmtData;
            Wavesample m_wavesample;
            std::vector<uint8_t> m_wavedata;
            std::vector<WavesampleLoop> m_loops;
        };

        /** \brief A Downloadable Sound is a collection of sampled instruments
         *
         * See Downloadable Sounds Level 1, Version 1.1b
         **/
        class DownloadableSound {
        public:
            
            DownloadableSound(DirectMusic::Riff::Chunk& c);
            const std::vector<Instrument>& getInstruments() const;
            const std::vector<std::uint32_t>& getPoolOffsets() const;
            const std::vector<Wave>& getWavePool() const;
            DirectMusic::Riff::Info& getInfo();
            const GUID& getGuid() const;

        private:
            std::uint64_t m_version;
            GUID m_dlsid;
            std::vector<Instrument> m_instruments;
            DirectMusic::Riff::Info m_info;
            std::vector<std::uint32_t> m_poolOffsets;
            std::vector<Wave> m_wavePool;
        };
    }
}

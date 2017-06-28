#pragma once

#include "Common.h"
#include "Instrument.h"
#include "../Guid.h"
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
            const Sampler& getSampler() const;
            const std::vector<uint8_t>& getWavedata() const;
            const std::vector<WavesampleLoop>& getWavesampleLoops() const;

            void writeToStream(std::ostream& stream) const;
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

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
            Wave(const DirectMusic::Riff::Chunk& c);
            const GUID& getGuid() const { return m_dlsid; }
            const DirectMusic::Riff::Info& getInfo() const { return m_info; }
            const WaveFormatEx& getWaveformat() const { return m_fmtex; }
            const Wavesample& getWavesample() const { return m_wavesample; }
            const std::vector<uint8_t>& getWavedata() const { return m_wavedata; }
            const std::vector<WavesampleLoop>& getWavesampleLoops() const { return m_loops; }

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
            DownloadableSound() {};
            DownloadableSound(const std::string& path);
            DownloadableSound(const DirectMusic::Riff::Chunk& c);
            const std::vector<Instrument>& getInstruments() const { return m_instruments; }
            const std::vector<std::uint32_t>& getPoolOffsets() const { return m_poolOffsets; }
            std::vector<Wave>& getWavePool() { return m_wavePool; }
            const DirectMusic::Riff::Info& getInfo() const { return m_info; }
            const GUID& getGuid() const { return m_dlsid; }

            bool operator==(const DownloadableSound& a) const;
            bool operator!=(const DownloadableSound& a) const { return !(*this == a); };

        private:
            std::uint64_t m_version;
            GUID m_dlsid;
            std::vector<Instrument> m_instruments;
            DirectMusic::Riff::Info m_info;
            std::vector<std::uint32_t> m_poolOffsets;
            std::vector<Wave> m_wavePool;

            void loadChunk(const DirectMusic::Riff::Chunk& c);
        };
    }
}

namespace std {
    template<> struct hash<DirectMusic::DLS::DownloadableSound> {
        typedef DirectMusic::DLS::DownloadableSound argument_type;
        typedef std::size_t result_type;
        result_type operator()(argument_type const& s) const noexcept {
            return std::hash<DirectMusic::GUID>{}(s.getGuid());
        }
    };
}

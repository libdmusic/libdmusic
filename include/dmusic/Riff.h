#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace DirectMusic {
    namespace Riff {
        // Represents a RIFF chunk, which may or may not contain other subchunks
        class Chunk final {
        public:
            Chunk(const std::uint8_t* buf);

            // Returns the raw data content of the chunk
            const std::vector<std::uint8_t>& getData() const;

            // Returns the FOURCC which identifies the chunk
            const std::string& getId() const;

            // Returns the FOURCC which identifies the chunk
            // in case it contains subchunks
            const std::string& getListId() const;

            // Returns the contained subchunks
            const std::vector<Chunk>& getSubchunks() const;

        private:
            std::vector<std::uint8_t> m_data;
            std::string m_id;
            std::string m_listId;
            std::vector<Chunk> m_subchunks;
        };

        class Info {
        public:
            Info() {}
            Info(Chunk& c);
            const std::string& getArchivalLocation() const;
            const std::string& getArtist() const;
            const std::string& getCommission() const;
            const std::string& getComments() const;
            const std::string& getCopyright() const;
            const std::string& getCreationDate() const;
            const std::string& getEngineer() const;
            const std::string& getGenre() const;
            const std::string& getKeywords() const;
            const std::string& getMedium() const;
            const std::string& getName() const;
            const std::string& getProduct() const;
            const std::string& getSubject() const;
            const std::string& getSoftware() const;
            const std::string& getSource() const;
            const std::string& getSourceForm() const;
            const std::string& getTechnician() const;

        private:
            std::string m_iarl, m_iart, m_icms, m_icmt,
                m_icop, m_icrd, m_ieng, m_ignr, m_ikey,
                m_imed, m_inam, m_iprd, m_isbj, m_isft,
                m_isrc, m_isrf, m_itch;
        };
    }
}

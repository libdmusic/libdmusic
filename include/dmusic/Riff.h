#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace DirectMusic {
    namespace Riff {
        // Represents a RIFF chunk, which may or may not contain other subchunks
        class Chunk final {
        public:
            Chunk(const std::vector<std::uint8_t>& buffer, std::uint32_t offset);

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
            Info(Chunk& c);
            const std::string& getArchivalLocation();
            const std::string& getArtist();
            const std::string& getCommission();
            const std::string& getComments();
            const std::string& getCopyright();
            const std::string& getCreationDate();
            const std::string& getEngineer();
            const std::string& getGenre();
            const std::string& getKeywords();
            const std::string& getMedium();
            const std::string& getName();
            const std::string& getProduct();
            const std::string& getSubject();
            const std::string& getSoftware();
            const std::string& getSource();
            const std::string& getSourceForm();
            const std::string& getTechnician();

        private:
            std::map<std::string, std::string> m_kvpairs;
        };
    }
}

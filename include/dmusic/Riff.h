#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <iostream>

namespace DirectMusic {
    namespace Riff {
        /// Represents a RIFF chunk, which may or may not contain other subchunks
        class Chunk final {
        public:
            Chunk(const std::uint8_t* buf);

            /// Returns the raw data content of the chunk
            const std::vector<std::uint8_t>& getData() const;

            /// Returns the FOURCC which identifies the chunk
            const std::string& getId() const;

            /// Returns the FOURCC which identifies the chunk in case it contains subchunks
            const std::string& getListId() const;

            /// Returns the contained subchunks
            const std::vector<Chunk>& getSubchunks() const;

        private:
            std::vector<std::uint8_t> m_data;
            std::string m_id;
            std::string m_listId;
            std::vector<Chunk> m_subchunks;
        };

        /// An helper class to aid parsing standard RIFF tags
        class Info {
        public:
            /// Creates an empty instance of the class
            Info() {}

            /** \brief Creates an instance of the class from the supplied chunk.
             *
             * The chunk must be a LIST chunk with an INFO listid.
             **/
            Info(Chunk& c);

            /// Returns the contents of the IARL tag
            const std::string& getArchivalLocation() const;

            /// Returns the contents of the IART tag
            const std::string& getArtist() const;

            /// Returns the contents of the ICMS tag
            const std::string& getCommission() const;

            /// Returns the contents of the ICMT tag
            const std::string& getComments() const;

            /// Returns the contents of the ICOP tag
            const std::string& getCopyright() const;

            /// Returns the contents of the ICRD tag
            const std::string& getCreationDate() const;

            /// Returns the contents of the IENG tag
            const std::string& getEngineer() const;

            /// Returns the contents of the IGNR tag
            const std::string& getGenre() const;

            /// Returns the contents of the IKEY tag
            const std::string& getKeywords() const;

            /// Returns the contents of the IMED tag
            const std::string& getMedium() const;

            /// Returns the contents of the INAM tag
            const std::string& getName() const;

            /// Returns the contents of the IPRD tag
            const std::string& getProduct() const;

            /// Returns the contents of the ISBJ tag
            const std::string& getSubject() const;
            
            /// Returns the contents of the ISFT tag
            const std::string& getSoftware() const;

            /// Returns the contents of the ISRC tag
            const std::string& getSource() const;

            /// Returns the contents of the ISRF tag
            const std::string& getSourceForm() const;

            /// Returns the contents of the ITCH tag
            const std::string& getTechnician() const;

        private:
            std::string m_iarl, m_iart, m_icms, m_icmt,
                m_icop, m_icrd, m_ieng, m_ignr, m_ikey,
                m_imed, m_inam, m_iprd, m_isbj, m_isft,
                m_isrc, m_isrf, m_itch;
        };
    }
}

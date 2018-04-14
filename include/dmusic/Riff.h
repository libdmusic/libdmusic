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
            const std::vector<std::uint8_t>& getData() const { return m_data; }

            /// Returns the FOURCC which identifies the chunk
            const std::string& getId() const { return m_id; }

            /// Returns the FOURCC which identifies the chunk in case it contains subchunks
            const std::string& getListId() const { return m_listId; }

            /// Returns the contained subchunks
            const std::vector<Chunk>& getSubchunks() const { return m_subchunks; }

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
            Info(): m_iarl(""), m_iart(""), m_icms(""), m_icmt(""),
                m_icop(""), m_icrd(""), m_ieng(""), m_ignr(""), m_ikey(""),
                m_imed(""), m_inam(""), m_iprd(""), m_isbj(""), m_isft(""),
                m_isrc(""), m_isrf(""), m_itch("") {}

            /** \brief Creates an instance of the class from the supplied chunk.
             *
             * The chunk must be a LIST chunk with an INFO listid.
             **/
            Info(const Chunk& c);

            /// Returns the contents of the IARL tag
            const std::string& getArchivalLocation() const { return m_iarl; }

            /// Returns the contents of the IART tag
            const std::string& getArtist() const { return m_iart; }

            /// Returns the contents of the ICMS tag
            const std::string& getCommission() const { return m_icms; }

            /// Returns the contents of the ICMT tag
            const std::string& getComments() const { return m_icmt; }

            /// Returns the contents of the ICOP tag
            const std::string& getCopyright() const { return m_icop; }

            /// Returns the contents of the ICRD tag
            const std::string& getCreationDate() const { return m_icrd; }

            /// Returns the contents of the IENG tag
            const std::string& getEngineer() const { return m_ieng; }

            /// Returns the contents of the IGNR tag
            const std::string& getGenre() const { return m_ignr; }

            /// Returns the contents of the IKEY tag
            const std::string& getKeywords() const { return m_ikey; }

            /// Returns the contents of the IMED tag
            const std::string& getMedium() const { return m_imed; }

            /// Returns the contents of the INAM tag
            const std::string& getName() const { return m_inam; }

            /// Returns the contents of the IPRD tag
            const std::string& getProduct() const { return m_iprd; }

            /// Returns the contents of the ISBJ tag
            const std::string& getSubject() const { return m_isbj; }

            /// Returns the contents of the ISFT tag
            const std::string& getSoftware() const { return m_isft; }

            /// Returns the contents of the ISRC tag
            const std::string& getSource() const { return m_isrc; }

            /// Returns the contents of the ISRF tag
            const std::string& getSourceForm() const { return m_isrf; }

            /// Returns the contents of the ITCH tag
            const std::string& getTechnician() const { return m_itch; }

            bool operator==(const Info& a) const {
                return a.m_iarl == m_iarl &&
                    a.m_iart == m_iart &&
                    a.m_icms == m_icms &&
                    a.m_icmt == m_icmt &&
                    a.m_icop == m_icop &&
                    a.m_icrd == m_icrd &&
                    a.m_ieng == m_ieng &&
                    a.m_ignr == m_ignr &&
                    a.m_ikey == m_ikey &&
                    a.m_imed == m_imed &&
                    a.m_inam == m_inam &&
                    a.m_iprd == m_iprd &&
                    a.m_isbj == m_isbj &&
                    a.m_isft == m_isft &&
                    a.m_isrc == m_isrc &&
                    a.m_isrf == m_isrf &&
                    a.m_itch == m_itch;
            }

        private:
            std::string m_iarl, m_iart, m_icms, m_icmt,
                m_icop, m_icrd, m_ieng, m_ignr, m_ikey,
                m_imed, m_inam, m_iprd, m_isbj, m_isft,
                m_isrc, m_isrf, m_itch;
        };

        /// An helper class to aid parsing Unicode-encoded RIFF tags
        class Unfo {
        public:
            /// Creates an empty instance of the class
            Unfo() : m_iarl(""), m_iart(""), m_icms(""), m_icmt(""),
                m_icop(""), m_icrd(""), m_ieng(""), m_ignr(""), m_ikey(""),
                m_imed(""), m_inam(""), m_iprd(""), m_isbj(""), m_isft(""),
                m_isrc(""), m_isrf(""), m_itch("") {}

            /** \brief Creates an instance of the class from the supplied chunk.
            *
            * The chunk must be a LIST chunk with an INFO listid.
            **/
            Unfo(const Chunk& c);

            /// Returns the contents of the IARL tag
            const std::string& getArchivalLocation() const { return m_iarl; }

            /// Returns the contents of the IART tag
            const std::string& getArtist() const { return m_iart; }

            /// Returns the contents of the ICMS tag
            const std::string& getCommission() const { return m_icms; }

            /// Returns the contents of the ICMT tag
            const std::string& getComments() const { return m_icmt; }

            /// Returns the contents of the ICOP tag
            const std::string& getCopyright() const { return m_icop; }

            /// Returns the contents of the ICRD tag
            const std::string& getCreationDate() const { return m_icrd; }

            /// Returns the contents of the IENG tag
            const std::string& getEngineer() const { return m_ieng; }

            /// Returns the contents of the IGNR tag
            const std::string& getGenre() const { return m_ignr; }

            /// Returns the contents of the IKEY tag
            const std::string& getKeywords() const { return m_ikey; }

            /// Returns the contents of the IMED tag
            const std::string& getMedium() const { return m_imed; }

            /// Returns the contents of the INAM tag
            const std::string& getName() const { return m_inam; }

            /// Returns the contents of the IPRD tag
            const std::string& getProduct() const { return m_iprd; }

            /// Returns the contents of the ISBJ tag
            const std::string& getSubject() const { return m_isbj; }

            /// Returns the contents of the ISFT tag
            const std::string& getSoftware() const { return m_isft; }

            /// Returns the contents of the ISRC tag
            const std::string& getSource() const { return m_isrc; }

            /// Returns the contents of the ISRF tag
            const std::string& getSourceForm() const { return m_isrf; }

            /// Returns the contents of the ITCH tag
            const std::string& getTechnician() const { return m_itch; }

            bool operator==(const Unfo& a) const {
                return a.m_iarl == m_iarl &&
                    a.m_iart == m_iart &&
                    a.m_icms == m_icms &&
                    a.m_icmt == m_icmt &&
                    a.m_icop == m_icop &&
                    a.m_icrd == m_icrd &&
                    a.m_ieng == m_ieng &&
                    a.m_ignr == m_ignr &&
                    a.m_ikey == m_ikey &&
                    a.m_imed == m_imed &&
                    a.m_inam == m_inam &&
                    a.m_iprd == m_iprd &&
                    a.m_isbj == m_isbj &&
                    a.m_isft == m_isft &&
                    a.m_isrc == m_isrc &&
                    a.m_isrf == m_isrf &&
                    a.m_itch == m_itch;
            }

        private:
            std::string m_iarl, m_iart, m_icms, m_icmt,
                m_icop, m_icrd, m_ieng, m_ignr, m_ikey,
                m_imed, m_inam, m_iprd, m_isbj, m_isft,
                m_isrc, m_isrf, m_itch;
        };
    }
}

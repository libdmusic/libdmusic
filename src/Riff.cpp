#include <dmusic/Riff.h>
#include <dmusic/Exceptions.h>
#include <string>

using namespace DirectMusic::Riff;

Chunk::Chunk(const std::vector<std::uint8_t>& buffer, std::uint32_t offset) {
    const uint8_t *bufferData = buffer.data();
    std::uint32_t position = offset;
    m_id = std::string((const char *)(bufferData + position), 4);
    position += 4;
    std::uint32_t len = (std::uint32_t)*(bufferData + position);
    position += 4;

    // A RIFF chunk only contains subchunks if its ID is "RIFF" or "LIST"
    if (!m_id.compare("RIFF") || !m_id.compare("LIST")) {
        m_listId = std::string((const char *)(bufferData + position), 4);
        position += 4;
        m_data = std::vector<std::uint8_t>(bufferData + position, bufferData + position + len);
        uint32_t suboffset = 0;

        // len includes the listId FOURCC, we must exclude it in this calculation
        while (suboffset < len - 4) {
            Chunk subchunk(m_data, suboffset);
            m_subchunks.push_back(subchunk);
            suboffset += subchunk.m_data.size() + 8;
        }
    } else {
        m_data = std::vector<std::uint8_t>(bufferData + position, bufferData + position + len);
    }
}

const std::string& Chunk::getId() const {
    return m_id;
}

const std::string& Chunk::getListId() const {
    return m_listId;
}

const std::vector<std::uint8_t>& Chunk::getData() const {
    return m_data;
}

const std::vector<Chunk>& Chunk::getSubchunks() const {
    return m_subchunks;
}

Info::Info(Chunk& c) {
    if (c.getId().compare("LIST") != 0 || c.getListId().compare("INFO") != 0)
        throw DirectMusic::InvalidChunkException("LIST INFO", c.getId() + " " + c.getListId());
    for(Chunk subchunk : c.getSubchunks()) {
        std::vector<std::uint8_t> data = subchunk.getData();
        m_kvpairs[subchunk.getId()] = std::string((const char *)data.data(), data.size());
    }
}

const std::string& Info::getArchivalLocation() {
    return m_kvpairs["IARL"];
}

const std::string& Info::getArtist() {
    return m_kvpairs["IART"];
}

const std::string& Info::getCommission() {
    return m_kvpairs["ICMS"];
}

const std::string& Info::getComments() {
    return m_kvpairs["ICMT"];
}

const std::string& Info::getCopyright() {
    return m_kvpairs["ICOP"];
}

const std::string& Info::getCreationDate() {
    return m_kvpairs["ICRD"];
}

const std::string& Info::getEngineer() {
    return m_kvpairs["IENG"];
}

const std::string& Info::getGenre() {
    return m_kvpairs["IGNR"];
}

const std::string& Info::getKeywords() {
    return m_kvpairs["IKEY"];
}

const std::string& Info::getMedium() {
    return m_kvpairs["IMED"];
}

const std::string& Info::getName() {
    return m_kvpairs["INAM"];
}

const std::string& Info::getProduct() {
    return m_kvpairs["IPRD"];
}

const std::string& Info::getSubject() {
    return m_kvpairs["ISBJ"];
}

const std::string& Info::getSoftware() {
    return m_kvpairs["ISFT"];
}

const std::string& Info::getSource() {
    return m_kvpairs["ISRC"];
}

const std::string& Info::getSourceForm() {
    return m_kvpairs["ISRF"];
}

const std::string& Info::getTechnician() {
    return m_kvpairs["ITCH"];
}

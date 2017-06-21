#include <dmusic/Riff.h>

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

const std::string Chunk::getId() const {
    return m_id;
}

const std::string Chunk::getListId() const {
    return m_listId;
}

const std::vector<std::uint8_t> Chunk::getData() const {
    return m_data;
}

const std::vector<Chunk> Chunk::getSubchunks() const {
    return m_subchunks;
}

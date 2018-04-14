#include <dmusic/Riff.h>
#include <dmusic/Exceptions.h>
#include <dmusic/Common.h>
#include <string>
#include <codecvt>
#include <locale>

using namespace DirectMusic::Riff;

struct ChunkHeader {
    char id[4];
    std::uint32_t size;

    ChunkHeader(const std::uint8_t *data) {
        READFOURCC(ChunkHeader, id);
        FIELDINIT(ChunkHeader, size, std::uint32_t);
    }
};

Chunk::Chunk(const std::uint8_t* buffer) {
    ChunkHeader header(buffer);
    m_id = std::string(header.id, 4);
    buffer += sizeof(ChunkHeader);
    m_data = std::vector<std::uint8_t>(buffer, buffer + header.size);

    // A RIFF chunk only contains subchunks if its ID is "RIFF" or "LIST"
    // BUT!! There is also the 'seqt' chunk which doesn't conform to this convention
    // and has subchunks nonetheless...
    if (m_id == "RIFF" || m_id == "LIST" || m_id == "seqt") {
        std::uint32_t sbchkSize = header.size;
        if (m_id != "seqt") {
            m_listId = std::string((const char *)buffer, 4);
            buffer += 4;
            sbchkSize -= 4;
        }
        std::uint32_t count = 0;
        while (count < sbchkSize) {
            Chunk subchunk(buffer);
            m_subchunks.push_back(subchunk);
            int size = subchunk.m_data.size();
            if (size % 2 == 1) size++;
            count += size + 8;
            buffer += size + 8;
        }
    }
}

Info::Info(const Chunk& c):
    m_iarl(""), m_iart(""), m_icms(""), m_icmt(""),
    m_icop(""), m_icrd(""), m_ieng(""), m_ignr(""), m_ikey(""),
    m_imed(""), m_inam(""), m_iprd(""), m_isbj(""), m_isft(""),
    m_isrc(""), m_isrf(""), m_itch("")
{
    if (c.getId() != "LIST" || c.getListId() != "INFO")
        throw DirectMusic::InvalidChunkException("LIST INFO", c.getId() + " " + c.getListId());
    for(const Chunk& subchunk : c.getSubchunks()) {
        std::vector<std::uint8_t> data = subchunk.getData();
        const std::string& id = subchunk.getId();
        std::string value = std::string((const char *)data.data());
        if (id == "IARL") m_iarl = value;
        if (id == "IART") m_iart = value;
        if (id == "ICMS") m_icms = value;
        if (id == "ICMT") m_icmt = value;
        if (id == "ICOP") m_icop = value;
        if (id == "ICRD") m_icrd = value;
        if (id == "IENG") m_ieng = value;
        if (id == "IGNR") m_ignr = value;
        if (id == "IKEY") m_ikey = value;
        if (id == "IMED") m_imed = value;
        if (id == "INAM") m_inam = value;
        if (id == "IPRD") m_iprd = value;
        if (id == "ISBJ") m_isbj = value;
        if (id == "ISFT") m_isft = value;
        if (id == "ISRC") m_isrc = value;
        if (id == "ISRF") m_isrf = value;
        if (id == "ITCH") m_itch = value;
    }
}

Unfo::Unfo(const Chunk& c) :
    m_iarl(""), m_iart(""), m_icms(""), m_icmt(""),
    m_icop(""), m_icrd(""), m_ieng(""), m_ignr(""), m_ikey(""),
    m_imed(""), m_inam(""), m_iprd(""), m_isbj(""), m_isft(""),
    m_isrc(""), m_isrf(""), m_itch("")
{
    if (c.getId() != "LIST" || c.getListId() != "UNFO")
        throw DirectMusic::InvalidChunkException("LIST UNFO", c.getId() + " " + c.getListId());
    for (const Chunk& subchunk : c.getSubchunks()) {
        std::vector<std::uint8_t> data = subchunk.getData();
        const std::string& id = subchunk.getId();

        std::string value = utf16_to_utf8((const std::uint16_t *)data.data());
        if (id == "UARL") m_iarl = value;
        if (id == "UART") m_iart = value;
        if (id == "UCMS") m_icms = value;
        if (id == "UCMT") m_icmt = value;
        if (id == "UCOP") m_icop = value;
        if (id == "UCRD") m_icrd = value;
        if (id == "UENG") m_ieng = value;
        if (id == "UGNR") m_ignr = value;
        if (id == "UKEY") m_ikey = value;
        if (id == "UMED") m_imed = value;
        if (id == "UNAM") m_inam = value;
        if (id == "UPRD") m_iprd = value;
        if (id == "USBJ") m_isbj = value;
        if (id == "USFT") m_isft = value;
        if (id == "USRC") m_isrc = value;
        if (id == "USRF") m_isrf = value;
        if (id == "UTCH") m_itch = value;
    }
}

#include <dmusic/dls/Instrument.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic::Riff;
using namespace DirectMusic::DLS;

Instrument::Instrument(const Chunk& c) {
    if (c.getId() != "LIST" || c.getListId() != "ins ")
        throw DirectMusic::InvalidChunkException("LIST ins", c.getId() + " " + c.getListId());

    for (const Chunk& subchunk : c.getSubchunks()) {
        const std::string& id = subchunk.getId();
        if (id == "dlid") {
            m_dlsid = *((GUID*)subchunk.getData().data());
        } else if (id == "insh") {
            InstrumentHeader header(subchunk.getData().data());
            m_midiBank = header.Locale.ulBank;
            m_midiProgram = header.Locale.ulInstrument;
        } else if (id == "LIST") {
            if (subchunk.getListId() == "INFO") {
                m_info = Info(subchunk);
            } else if (subchunk.getListId() == "lrgn") {
                for (Chunk rgn : subchunk.getSubchunks()) {
                    m_regions.push_back(Region(rgn));
                }
            } else if (subchunk.getListId() == "lart") {
                for (Chunk art : subchunk.getSubchunks()) {
                    m_articulators.push_back(Articulator(art));
                }
            }
        }
    }
}
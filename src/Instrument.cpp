#include <dmusic/dls/Instrument.h>
#include <dmusic/Exceptions.h>

using namespace DirectMusic::Riff;
using namespace DirectMusic::DLS;

Instrument::Instrument(Chunk& c) {
    if (c.getId().compare("LIST") != 0 || c.getListId().compare("ins ") != 0)
        throw DirectMusic::InvalidChunkException("LIST ins", c.getId() + " " + c.getListId());

    for (Chunk subchunk : c.getSubchunks()) {
        if (!subchunk.getId().compare("dlid")) {
            m_dlsid = *((GUID*)subchunk.getData().data());
        } else if (!subchunk.getId().compare("insh")) {
            InstrumentHeader *header = (InstrumentHeader*)subchunk.getData().data();
            m_midiBank = header->Locale.ulBank;
            m_midiProgram = header->Locale.ulInstrument;
        } else if (!subchunk.getId().compare("LIST")) {
            if (!subchunk.getListId().compare("INFO")) {
                m_info = Info(subchunk);
            } else if (!subchunk.getId().compare("lrgn")) {
                for (Chunk rgn : subchunk.getSubchunks()) {
                    m_regions.push_back(Region(rgn));
                }
            } else if (!subchunk.getId().compare("lart")) {
                for (Chunk art : subchunk.getSubchunks()) {
                    m_articulators.push_back(Articulator(art));
                }
            }
        }
    }
}

const std::vector<Region>& Instrument::getRegions() const {
    return m_regions;
}

const std::vector<Articulator>& Instrument::getArticulators() const {
    return m_articulators;
}

const std::uint32_t Instrument::getMidiBank() const {
    return m_midiBank;
}

const std::uint32_t Instrument::getMidiProgram() const {
    return m_midiBank;
}

const Info& Instrument::getInfo() const {
    return m_info;
}

const DirectMusic::GUID& Instrument::getGuid() const {
    return m_dlsid;
}

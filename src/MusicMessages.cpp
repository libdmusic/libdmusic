#include "MusicMessages.h"
#include <dmusic/PlayingContext.h>
#include <assert.h>

using namespace DirectMusic;

void MusicMessage::changeTempo(PlayingContext& ctx, double tempo) {
    ctx.m_tempo = tempo;
}

std::shared_ptr<InstrumentPlayer> MusicMessage::createInstrument(PlayingContext& ctx, std::uint8_t bank_lo, std::uint8_t bank_hi, std::uint8_t patch, const DirectMusic::DLS::DownloadableSound& dls) {
    return ctx.m_instrumentFactory(bank_lo, bank_hi, patch, dls, ctx.m_sampleRate, ctx.m_audioChannels);
}

void MusicMessage::setInstrument(PlayingContext& ctx, std::uint32_t channel, std::shared_ptr<InstrumentPlayer> instr) {
    ctx.m_performanceChannels[channel] = instr;
}

void TempoChangeMessage::Execute(PlayingContext& ctx) {
    this->changeTempo(ctx, m_tempo);
}

BandChangeMessage::BandChangeMessage(PlayingContext& ctx, std::uint32_t time, const BandForm& form)
    : MusicMessage(time)
{
    for (const auto& instr : form.getInstruments()) {
        const auto& header = instr.getHeader();
        const auto ref = instr.getReference();
        if (ref != nullptr) {
            std::uint8_t bankHi = (header.dwPatch & 0x00FF0000) >> 0x10;
            std::uint8_t bankLo = (header.dwPatch & 0x0000FF00) >> 0x8;
            std::uint8_t patch = (header.dwPatch & 0x000000FF);

            using convert_type = std::codecvt_utf8<wchar_t>;
            std::wstring_convert<convert_type, wchar_t> converter;
            std::string converted_str = converter.to_bytes(ref->getFile());
            auto dls = ctx.loadInstrumentCollection(converted_str);
            
            assert(dls != nullptr);
            instruments[header.dwPChannel] = createInstrument(ctx, bankLo, bankHi, patch, *dls);
        }
    }
}

void BandChangeMessage::Execute(PlayingContext& ctx) {
    for (const auto& kvpair : instruments) {
        setInstrument(ctx, kvpair.first, kvpair.second);
    }
}
#if DMUSIC_TSF_SUPPORT
#include <cassert>
#include <exception>
#include <dmusic/SoundFontPlayer.h>
#include <memory>
#include <cmath>
#define TSF_IMPLEMENTATION
#include "../utils/common/tsf.h"
using namespace DirectMusic;
using namespace DirectMusic::DLS;

static float gainToDecibels(float gain) {
    return 10 * log10(gain);
}

SoundFontPlayer::SoundFontPlayer(tsf* soundfont,
    std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
    DirectMusic::DLS::DownloadableSound& dls,
    std::uint32_t sampleRate,
    std::uint32_t channels,
    float volume,
    float pan)
    : InstrumentPlayer(bankLo, bankHi, patch, dls, sampleRate, channels, volume, pan)
    , m_soundfont(nullptr) {
    assert(channels <= 2);
    std::uint32_t bank = (bankHi << 16) + bankLo;

    m_pan = pan < -1 ? -1 : pan > 1 ? 1 : pan;
    m_soundfont = soundfont;

    m_preset = tsf_get_presetindex(m_soundfont, 0, patch);
    assert(m_preset >= 0);

    float volFactorRight = sqrt((m_pan + 1) / 2);
    float volFactorLeft = sqrt((-m_pan + 1) / 2);

    tsf_set_preset_panning(m_soundfont, m_preset, volFactorLeft, volFactorRight);
    tsf_set_preset_gain(m_soundfont, m_preset, 0);
}

SoundFontPlayer::~SoundFontPlayer() {
}

std::uint32_t SoundFontPlayer::renderBlock(std::int16_t *buffer, std::uint32_t count, bool mix) noexcept {
    if (!mix) {
        tsf_render_short(m_soundfont, buffer, count / m_channels, 0);
    }
    return count;
}

/// Instructs the synthesizer to start playing a note
void SoundFontPlayer::noteOn(std::uint8_t note, std::uint8_t velocity) {
    tsf_note_on(m_soundfont, m_preset, note, velocity / 255.0f);
}

/// Instructs the synthesizer to stop playing a note
void SoundFontPlayer::noteOff(std::uint8_t note, std::uint8_t velocity) {
    tsf_note_off(m_soundfont, m_preset, note);
}

void SoundFontPlayer::allNotesOff() {
    tsf_all_notes_off(m_soundfont, m_preset);
}

/// Sends a "channel pressure" message
void SoundFontPlayer::channelPressure(std::uint8_t val) {}

/// Sends a "polyphonic aftertouch" message to a note
void SoundFontPlayer::polyAftertouch(std::uint8_t note, std::uint8_t val) {}

/// Sends a "control change" message
void SoundFontPlayer::controlChange(DirectMusic::Midi::Control control, float val) {}

/// Sends a "program change" (aka "patch change") message
void SoundFontPlayer::programChange(std::uint8_t program) {}

/// Sends a "pitch bend" message
void SoundFontPlayer::pitchBend(std::int16_t val) {}

PlayerFactory SoundFontPlayer::createFactory(const std::string& file) {
    tsf* soundfont = tsf_load_filename(file.c_str());
    if (soundfont == nullptr) {
        throw std::runtime_error("Cannot open " + file);
    }

    return [soundfont](std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
        const GUID& bandGuid, DownloadableSound& dls, std::uint32_t sampleRate, std::uint32_t chans, float vol, float pan) {

        TSFOutputMode outputMode = chans == 1 ? TSF_MONO : TSF_STEREO_INTERLEAVED;
        tsf_set_output(soundfont, outputMode, sampleRate, 0);

        return std::static_pointer_cast<InstrumentPlayer>(std::shared_ptr<SoundFontPlayer>{
            new SoundFontPlayer(soundfont, bankLo, bankHi, patch, dls, sampleRate, chans, vol, pan)
        });
    };
}

PlayerFactory SoundFontPlayer::createMultiFactory(const std::string dir) {
    std::shared_ptr<std::map<GUID, tsf*>> soundfonts = std::make_shared<std::map<GUID, tsf*>>();
    return [soundfonts, dir](std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
        const GUID& bandGuid, DownloadableSound& dls, std::uint32_t sampleRate, std::uint32_t chans, float vol, float pan) {
        tsf* soundfont;
        if (soundfonts->find(bandGuid) == soundfonts->end()) {
            auto file = dir + "/" + bandGuid.toString() + ".sf2";
            soundfont = tsf_load_filename(file.c_str());
            if (soundfont == nullptr) {
                throw std::runtime_error("Cannot open " + file);
            }
            soundfonts->operator[](bandGuid) = soundfont;
        } else {
            soundfont = soundfonts->at(bandGuid);
        }

        TSFOutputMode outputMode = chans == 1 ? TSF_MONO : TSF_STEREO_INTERLEAVED;
        tsf_set_output(soundfont, outputMode, sampleRate, 0);

        return std::static_pointer_cast<InstrumentPlayer>(std::shared_ptr<SoundFontPlayer>{
            new SoundFontPlayer(soundfont, bankLo, bankHi, patch, dls, sampleRate, chans, vol, pan)
        });
    };
}

#endif
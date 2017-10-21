#include <iostream>
#include <queue>
#include <locale>
#include <map>
#include <dmusic/PlayingContext.h>
#include <dmusic/InstrumentPlayer.h>
#include <dmusic/Tracks.h>
#include <dmusic/dls/DownloadableSound.h>
#include <cassert>
#include <sndfile.h>
#include <cmath>
#define TSF_IMPLEMENTATION
#include "tsf.h"

using namespace DirectMusic;
using namespace DirectMusic::DLS;

class MyInstrumentPlayer : public InstrumentPlayer {
private:
    int m_preset;
    tsf* m_soundfont;
    float m_vol;
    float m_pan;
    int m_channels;
    float gainToDecibels(float gain) {
        return 10 * log10(gain);
    }
public:
    MyInstrumentPlayer(std::map<std::string, tsf*> soundfonts,
        std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
        const DownloadableSound& dls,
        std::uint32_t sampleRate,
        std::uint32_t channels,
        float volume,
        float pan)
        : InstrumentPlayer(bankLo, bankHi, patch, dls, sampleRate, channels, volume, pan)
        , m_soundfont(nullptr)
        , m_vol(volume)
        , m_pan(pan)
        , m_channels(channels) {
        assert(channels <= 2);
        std::uint32_t bank = (bankHi << 16) + bankLo;

        std::string soundfontFile = (dls.getInfo().getName() + ".sf2");
        if (soundfonts.find(soundfontFile) == soundfonts.end()) {
            m_soundfont = tsf_load_filename(soundfontFile.c_str());
            assert(m_soundfont != nullptr);
            TSFOutputMode outputMode = channels == 1 ? TSF_MONO : TSF_STEREO_INTERLEAVED;
            tsf_set_output(m_soundfont, outputMode, sampleRate, gainToDecibels(volume) - 10);

            soundfonts[soundfontFile] = m_soundfont;
        } else {
            m_soundfont = soundfonts.at(soundfontFile);
        }
        m_preset = tsf_get_presetindex(m_soundfont, 0, patch);
        m_preset = 0;
        assert(m_preset >= 0);
    }

    virtual std::uint32_t renderBlock(std::int16_t *buffer, std::uint32_t count, float volume) noexcept {
        tsf_render_short(m_soundfont, buffer, count / m_channels, 1);
        return count;
    }

    /// Instructs the synthesizer to start playing a note
    virtual void noteOn(std::uint8_t note, std::uint8_t velocity) {
        tsf_note_on(m_soundfont, m_preset, note, velocity / 255.0f);
    }

    /// Instructs the synthesizer to stop playing a note
    virtual void noteOff(std::uint8_t note, std::uint8_t velocity) {
        tsf_note_off(m_soundfont, m_preset, note);
    }

    /// Sends a "channel pressure" message
    virtual void channelPressure(std::uint8_t val) {}

    /// Sends a "polyphonic aftertouch" message to a note
    virtual void polyAftertouch(std::uint8_t note, std::uint8_t val) {}

    /// Sends a "control change" message
    virtual void controlChange(DirectMusic::Midi::Control control, std::int32_t val) {}

    /// Sends a "program change" (aka "patch change") message
    virtual void programChange(std::uint8_t program) {}

    /// Sends a "pitch bend" message
    virtual void pitchBend(std::int16_t val) {}
};

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Usage: dmrender [inputfile] [outputfile] <length in seconds>\n";
        return 1;
    }

    // Store soundfonts based on their name
    std::map<std::string, tsf*> soundfonts;
    PlayingContext ctx(44100, 2, [soundfonts](std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
        const DownloadableSound& dls, std::uint32_t sampleRate, std::uint32_t channels, float vol, float pan) {
        return std::static_pointer_cast<InstrumentPlayer>(std::make_shared<MyInstrumentPlayer>(soundfonts, bankLo, bankHi, patch, dls, sampleRate, channels, vol, pan));
    });
    std::cout << "Loading segment...";
    auto segment = ctx.loadSegment(argv[1]);
    std::cout << " done.\nBeginning rendering... ";

    ctx.playSegment(*segment);
    int sampleRate = 44100;
    std::uint64_t length = 60 * sampleRate * 2; // Render 60 seconds of sound if nothing else is specified, 2 channels
    if (argc > 3) {
        length = std::stoi(argv[3]) * sampleRate * 2;
    }

    // Each instrument is going to sum its output into the buffer,
    // so we have to start from a blank state, hence calloc
    std::int16_t* buffer = (std::int16_t*)calloc(length, sizeof(std::int16_t));
    for (std::uint64_t i = 0; i < length; i += sampleRate) {
        ctx.renderBlock(buffer + i, sampleRate);
        std::cout << ceil((i / (float)length) * 100) << "% ";
    }
    
    // Close all soundfont handles
    for (const auto& kvp : soundfonts) {
        tsf_close(kvp.second);
    }

    SF_INFO info;
    info.channels = 2;
    info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    info.samplerate = 44100;
    info.frames = 0;
    info.sections = 0;
    info.seekable = 0;
    SNDFILE* sndfile = sf_open(argv[2], SFM_WRITE, &info);
    sf_write_short(sndfile, buffer, length);
    sf_close(sndfile);
    free(buffer);
    std::cout << "Rendering done.";
    return 0;
}

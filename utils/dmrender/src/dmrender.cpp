#include <iostream>
#include <queue>
#include <locale>
#include <dmusic/PlayingContext.h>
#include <dmusic/InstrumentPlayer.h>
#include <dmusic/Tracks.h>
#include <dmusic/dls/DownloadableSound.h>
#include <cassert>
#include <fstream>
#define TSF_IMPLEMENTATION
#include "tsf.h"

using namespace DirectMusic;
using namespace DirectMusic::DLS;

static std::queue<std::string> styles;

class MyInstrumentPlayer : public InstrumentPlayer {
private:
    tsf* m_soundfont;
    std::uint32_t bank, preset;
public:
    MyInstrumentPlayer(std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
        const DownloadableSound& dls,
        std::uint32_t sampleRate,
        std::uint32_t channels) : InstrumentPlayer(bankLo, bankHi, patch, dls, sampleRate, channels) {
        assert(channels == 1);

        std::uint32_t bank = (bankHi << 16) + bankLo;

        for (const auto& instr : dls.getInstruments()) {
            if (/*instr.getMidiBank() == bank && */instr.getMidiProgram() == patch) {
                std::cout << "Instrument loaded: " << instr.getInfo().getName() << " from " << dls.getInfo().getName() << "\n";
                std::string soundfontFile = (dls.getInfo().getName() + ".sf2");
                m_soundfont = tsf_load_filename(soundfontFile.c_str());

                this->bank = bank;
                this->preset = patch;
                // FIXME: Only mono is supported
                tsf_set_output(m_soundfont, TSF_MONO, sampleRate);
                return;
            }
        }

        std::cout << "error";
    }

    virtual std::uint32_t renderBlock(std::int16_t *buffer, std::uint32_t count) noexcept {
        tsf_render_short(m_soundfont, buffer, count, 1);
        return count;
    }

    /// Instructs the synthesizer to start playing a note
    virtual void noteOn(std::uint8_t note, std::uint8_t velocity) {
        tsf_note_on(m_soundfont, preset, note, velocity / 255.0f);
    }

    /// Instructs the synthesizer to stop playing a note
    virtual void noteOff(std::uint8_t note, std::uint8_t velocity) {
        tsf_note_off(m_soundfont, preset, note);
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
        std::cerr << "Usage: dmrender [inputfile] [outputfile]\n";
        return 1;
    }
    PlayingContext ctx(44100, 1, [](std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
        const DownloadableSound& dls, std::uint32_t sampleRate, std::uint32_t channels) {
        return std::static_pointer_cast<InstrumentPlayer>(std::make_shared<MyInstrumentPlayer>(bankLo, bankHi, patch, dls, sampleRate, channels));
    });
    std::cout << "Loading segment...";
    auto segment = ctx.loadSegment(argv[1]);

    ctx.playSegment(*segment);
    int sampleRate = 44100;
    int length = 60 * sampleRate;
    std::vector<std::int16_t> buffer;
    buffer.reserve(length);
    ctx.renderBlock(buffer.data(), length);

    std::ofstream ofs(argv[2], std::ios::binary);
    writeWave(buffer, ofs);

    int a;
    std::cin >> a;
    return 0;
}

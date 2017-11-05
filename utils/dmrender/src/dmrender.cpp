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
#include "args.hxx"

using namespace DirectMusic;
using namespace DirectMusic::DLS;

class MyInstrumentPlayer : public InstrumentPlayer {
private:
    int m_preset;
    tsf* m_soundfont;
    float m_vol;
    float m_pan;
    int m_channels;
    int m_samplerate;
    bool m_singleFont;
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
        , m_channels(channels)
        , m_samplerate(sampleRate)
        , m_singleFont(false) {
        assert(channels <= 2);
        std::uint32_t bank = (bankHi << 16) + bankLo;

        std::string soundfontFile = (dls.getInfo().getName() + ".sf2");
        if (soundfonts.find(soundfontFile) == soundfonts.end()) {
            m_soundfont = tsf_load_filename(soundfontFile.c_str());
            assert(m_soundfont != nullptr);
            TSFOutputMode outputMode = channels == 1 ? TSF_MONO : TSF_STEREO_INTERLEAVED;
            tsf_set_output(m_soundfont, outputMode, sampleRate, gainToDecibels(volume) - 3);
            pan = pan < -1 ? -1 : pan > 1 ? 1 : pan;
            float volFactorRight = sqrt((pan + 1) / 2);
            float volFactorLeft = sqrt((-pan + 1) / 2);
            tsf_set_panning(m_soundfont, volFactorLeft, volFactorRight);

            soundfonts[soundfontFile] = m_soundfont;
        } else {
            m_soundfont = soundfonts.at(soundfontFile);
        }
        m_preset = tsf_get_presetindex(m_soundfont, 0, patch);
        assert(m_preset >= 0);
    }

    MyInstrumentPlayer(tsf* soundfont,
        std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
        const DownloadableSound& dls,
        std::uint32_t sampleRate,
        std::uint32_t channels,
        float volume,
        float pan)
        : InstrumentPlayer(bankLo, bankHi, patch, dls, sampleRate, channels, volume, pan)
        , m_soundfont(nullptr)
        , m_vol(volume)
        , m_channels(channels)
        , m_singleFont(true) {
        assert(channels <= 2);
        std::uint32_t bank = (bankHi << 16) + bankLo;

        m_pan = pan < -1 ? -1 : pan > 1 ? 1 : pan;
        m_soundfont = soundfont;

        m_preset = tsf_get_presetindex(m_soundfont, 0, patch);
        assert(m_preset >= 0);

        float volFactorRight = sqrt((m_pan + 1) / 2);
        float volFactorLeft = sqrt((-m_pan + 1) / 2);

        tsf_set_preset_panning(m_soundfont, m_preset, volFactorLeft, volFactorRight);
        tsf_set_preset_gain(m_soundfont, m_preset, gainToDecibels(volume));
    }

    virtual std::uint32_t renderBlock(std::int16_t *buffer, std::uint32_t count, float volume, bool mix) noexcept {
        if (m_singleFont) {
            if (!mix) {
                tsf_render_short(m_soundfont, buffer, count / m_channels, 0);
            }
        } else {
            tsf_render_short(m_soundfont, buffer, count / m_channels, mix ? 1 : 0);
        }
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
    args::ArgumentParser parser("dmrender renders DirectMusic segments into audio files");
    args::HelpFlag help(parser, "help", "Display this help menu", { 'h', "help" });
    args::ValueFlag<int> chunkLength(parser, "length", "The length in seconds of the audio to render", { 'l', "length" });
    args::ValueFlag<int> samplingRate(parser, "sampling rate", "The sampling rate to use", { 's', "sample" });
    args::ValueFlag<int> numChannels(parser, "channels", "The number of channels to use", { 'c', "channels" });
    args::ValueFlag<std::string> sfont(parser, "soundfont", "The SoundFont file to use during rendering", { 'f', "soundfont" });
    args::Flag vorbis(parser, "ogg vorbis", "The output file is going to be an Ogg/Vorbis file instead of an uncompressed Microsoft WAVE file", { 'O', "ogg" });
    args::Positional<std::string> segmentName(parser, "segment", "The segment to render");
    args::Positional<std::string> outputFile(parser, "output", "The output file");

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help) {
        std::cout << parser;
        return 0;
    } catch (args::ParseError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    } catch (args::ValidationError e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    if (!segmentName) {
        std::cerr << "dmrender: No input specified." << std::endl;
        return 1;
    }
   
    if (!outputFile) {
        std::cerr << "dmrender: No output file specified" << std::endl;
        return 1;
    }

    int sampleRate = samplingRate ? args::get(samplingRate) : 44100;
    int channels = numChannels ? args::get(numChannels) : 1;
    std::uint64_t length = (chunkLength ? args::get(chunkLength) : 60) * sampleRate;

    // Store soundfonts based on their name
    tsf* soundfont = nullptr;
    std::map<std::string, tsf*> soundfontMap;
    std::unique_ptr<PlayingContext> ctx = nullptr;

    if (sfont) {
        soundfont = tsf_load_filename(args::get(sfont).c_str());

        TSFOutputMode outputMode = channels == 1 ? TSF_MONO : TSF_STEREO_INTERLEAVED;
        tsf_set_output(soundfont, outputMode, sampleRate, -3);

        ctx = std::make_unique<PlayingContext>(sampleRate, channels, [soundfont](std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
            const DownloadableSound& dls, std::uint32_t sampleRate, std::uint32_t chans, float vol, float pan) {
            return std::static_pointer_cast<InstrumentPlayer>(std::make_shared<MyInstrumentPlayer>(soundfont, bankLo, bankHi, patch, dls, sampleRate, chans, vol, pan));
        });
    } else {
        ctx = std::make_unique<PlayingContext>(sampleRate, channels, [soundfontMap](std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
            const DownloadableSound& dls, std::uint32_t sampleRate, std::uint32_t chans, float vol, float pan) {
            return std::static_pointer_cast<InstrumentPlayer>(std::make_shared<MyInstrumentPlayer>(soundfontMap, bankLo, bankHi, patch, dls, sampleRate, chans, vol, pan));
        });
    }
    std::cout << "Loading segment...";
    auto segment = ctx->loadSegment(args::get(segmentName));
    std::cout << " done.\nStart playback... ";
    ctx->playSegment(*segment);
    std::cout << " done.\nBegin rendering... ";

    // Each instrument is going to sum its output into the buffer,
    // so we have to start from a blank state, hence calloc
    std::int16_t* buffer = (std::int16_t*)malloc(length * sizeof(std::int16_t));
    for (std::uint64_t i = 0; i < length; i += sampleRate) {
        ctx->renderBlock(buffer + i, sampleRate);
        std::cout << ceil((i / (float)length) * 100) << "% ";
    }
    
    // Close all soundfont handles
    for (const auto& kvp : soundfontMap) {
        tsf_close(kvp.second);
    }
    if (soundfont != nullptr) {
        tsf_close(soundfont);
    }

    SF_INFO info;
    info.channels = channels;
    info.format = vorbis ? SF_FORMAT_OGG | SF_FORMAT_VORBIS : SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    info.samplerate = sampleRate;
    info.frames = 0;
    info.sections = 0;
    info.seekable = 0;
    SNDFILE* sndfile = sf_open(args::get(outputFile).c_str(), SFM_WRITE, &info);
    if (sndfile == nullptr) {
        std::string err = std::string(sf_strerror(sndfile));
        std::cerr << "Error encountered while opening file: " << err << std::endl;
        return 1;
    }
    sf_write_short(sndfile, buffer, length);
    sf_close(sndfile);
    free(buffer);
    std::cout << "Rendering done.";
    return 0;
}

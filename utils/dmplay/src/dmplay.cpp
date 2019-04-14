#include <iostream>
#include <queue>
#include <locale>
#include <map>
#include <dmusic/PlayingContext.h>
#include <dmusic/InstrumentPlayer.h>
#include <dmusic/DlsPlayer.h>
#include <dmusic/Tracks.h>
#include <dmusic/dls/DownloadableSound.h>
#include <cmath>
#include <cstdio>
#include <args.hxx>

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

using namespace DirectMusic;
using namespace DirectMusic::DLS;

static int streamChannels = 2;

static void audioCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
    /* Cast data passed through stream to our structure. */
    PlayingContext* data = (PlayingContext*)pDevice->pUserData;
    std::int16_t *out = (std::int16_t*)pOutput;
    int samples = frameCount * data->getAudioChannels();

    if(streamChannels > 2) {
        for(std::size_t i = 0; i < frameCount; i++) {
            data->renderBlock(out, 2, 1);
            std::fill(out + 2, out + streamChannels, 0);
            out += streamChannels;
        }
    } else {
        data->renderBlock(out, samples, 1);
    }
}

int main(int argc, char **argv) {
    args::ArgumentParser parser("dmplay plays DirectMusic segments in real time");
    args::HelpFlag help(parser, "help", "Display this help menu", { 'h', "help" });
    args::ValueFlag<unsigned int> samplingRate(parser, "sampling rate", "The sampling rate to use", { 's', "sample" });
    args::ValueFlag<unsigned int> numChannels(parser, "channels", "The number of channels to use", { 'c', "channels" });
    args::ValueFlag<std::string> gmDls(parser, "GeneralMidi font", "The DLS to use when playing GeneralMidi instruments", { "gm" });
    args::Positional<std::string> segmentName(parser, "segment", "The segment to render");

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
        std::cerr << "dmplay: No input specified." << std::endl;
        return 1;
    }


    int sampleRate = samplingRate ? args::get(samplingRate) : 44100;
    int channels = numChannels ? args::get(numChannels) : 2;

    DirectMusic::GMPlayerFactory gmFactory = nullptr;
    std::unique_ptr<DownloadableSound> dls = nullptr;
    if(gmDls) {
        dls = std::make_unique<DownloadableSound>(args::get(gmDls));
        gmFactory = DlsPlayer::createGMFactory(*dls);
    }

    PlayingContext ctx(sampleRate, channels > 2 ? 2 : channels, DlsPlayer::createFactory(), gmFactory);
    streamChannels = channels;

    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format   = ma_format_s16;
    config.playback.channels = channels;
    config.sampleRate        = sampleRate;
    config.dataCallback      = audioCallback;
    config.pUserData         = &ctx;

    ma_device device;
    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        std::cerr << "Failed to open playback device.\n";
        return -3;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cerr << "Failed to start playback device.\n";
        ma_device_uninit(&device);
        return -4;
    }

    std::cout << " done.\nLoading segment...";
    auto segment = ctx.loadSegment(args::get(segmentName));
    if (segment != nullptr) {
        std::cout << " done.\nStart playback... ";
        try {
            ctx.playSegment(*segment, SegmentTiming::Measure);
            std::cout << " done.\nBegin rendering... ";
        } catch (const std::runtime_error& e) {
            std::cerr << " Cannot play segment. " << e.what();
        }
    } else {
        std::cerr << "Cannot load segment.\n";
    }

    std::cout << "Rendering started. Insert the next segment to be played, or 'exit' to end playback.\n";
    while (true) {
        std::string input;
        std::cin >> input;
        if (input == "exit") {
            break;
        }
        std::cout << "Loading segment...";
        segment = ctx.loadSegment(input);
        if (segment == nullptr) {
            std::cerr << "Cannot load segment.\n";
            continue;
        }
        std::cout << " done.\nStart playback... ";
        try {
            ctx.playSegment(*segment, SegmentTiming::Measure);
        } catch (const std::runtime_error& e) {
            std::cerr << " Cannot play segment. " << e.what();
            return 1;
        }
        std::cout << " done.\nBegin rendering... ";
    }

    ma_device_uninit(&device);

    return 0;
}

#include <iostream>
#include <queue>
#include <locale>
#include <map>
#include <dmusic/PlayingContext.h>
#include <dmusic/InstrumentPlayer.h>
#include <dmusic/DlsPlayer.h>
#include <dmusic/Tracks.h>
#include <dmusic/dls/DownloadableSound.h>
#include <RtAudio.h>
#include <cmath>
#include <cstdio>
#include <args.hxx>

using namespace DirectMusic;
using namespace DirectMusic::DLS;

static int streamChannels = 2;

static int audioCallback(void* outputBuffer,
                      void* /*inputBuffer*/,
                      unsigned int framesPerBuffer,
                      double /*streamTime*/,
                      RtAudioStreamStatus /*status*/,
                      void* userData) {
    /* Cast data passed through stream to our structure. */
    PlayingContext* data = (PlayingContext*)userData;
    std::int16_t *out = (std::int16_t*)outputBuffer;
    int samples = framesPerBuffer * data->getAudioChannels();

    if(streamChannels > 2) {
        for(std::size_t i = 0; i < framesPerBuffer; i++) {
            data->renderBlock(out, 2, 1);
            std::fill(out + 2, out + streamChannels, 0);
            out += streamChannels;
        }
    } else {
        data->renderBlock(out, samples, 1);
    }
    return 0;
}

int main(int argc, char **argv) {
    args::ArgumentParser parser("dmplay plays DirectMusic segments in real time");
    args::HelpFlag help(parser, "help", "Display this help menu", { 'h', "help" });
    args::ValueFlag<unsigned int> samplingRate(parser, "sampling rate", "The sampling rate to use", { 's', "sample" });
    args::ValueFlag<unsigned int> numChannels(parser, "channels", "The number of channels to use", { 'c', "channels" });
    args::Flag listDevices(parser, "list devices", "Lists the available sound devices", { 'l', "list-devices" });
    args::ValueFlag<unsigned int> device(parser, "device", "Use a specific sound device for playback", {'d', "device"});
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

    RtAudio dac;
    if (dac.getDeviceCount() < 1) {
        std::cout << "No audio devices found!\n";
        return 1;
    }

    if(listDevices) {
        for (unsigned int i = 0; i < dac.getDeviceCount(); i++) {
            std::cout << i << ')';
            const auto& info = dac.getDeviceInfo(i);
            if (info.isDefaultOutput) std::cout << " (default output)";
            if (info.isDefaultInput) std::cout << " (default input)";
            std::cout << ' ' << info.name << '\n';
        }
        return 0;
    }

    if (!segmentName) {
        std::cerr << "dmplay: No input specified." << std::endl;
        return 1;
    }

    int sampleRate = samplingRate ? args::get(samplingRate) : 44100;
    int channels = numChannels ? args::get(numChannels) : 2;

    RtAudio::StreamParameters parameters;
    parameters.deviceId = device ? args::get(device) : dac.getDefaultOutputDevice();
    parameters.nChannels = channels;
    parameters.firstChannel = 0;

    unsigned int bufferFrames = 256;

    PlayingContext ctx(sampleRate, channels > 2 ? 2 : channels, DlsPlayer::createFactory());
    streamChannels = channels;

    std::cout << "Opening audio device... ";
    try {
        dac.openStream(&parameters, nullptr, RTAUDIO_SINT16,
            sampleRate, &bufferFrames, &audioCallback, (void *)&ctx);
        dac.startStream();
    } catch (const RtAudioError& e) {
        std::cerr << e.what() << '\n';
        return 1;
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
    
    try {
        // Stop the stream
        dac.stopStream();
    } catch (RtAudioError& e) {
        e.printMessage();
    }
    if ( dac.isStreamOpen() ) dac.closeStream();

    return 0;
}

#include <iostream>
#include <queue>
#include <locale>
#include <map>
#include <dmusic/PlayingContext.h>
#include <dmusic/InstrumentPlayer.h>
#ifndef DMUSIC_DLS_PLAYER
#define DMUSIC_DLS_PLAYER 1
#endif
#include <dmusic/DlsPlayer.h>
#include <dmusic/Tracks.h>
#include <dmusic/dls/DownloadableSound.h>
#include <cassert>
#include <RtAudio.h>
#include <cmath>
#include <cstdio>
#include <args.hxx>

using namespace DirectMusic;
using namespace DirectMusic::DLS;

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

    data->renderBlock(out, samples, 1);
    return 0;
}

int main(int argc, char **argv) {
    args::ArgumentParser parser("dmplay plays DirectMusic segments in real time");
    args::HelpFlag help(parser, "help", "Display this help menu", { 'h', "help" });
    args::ValueFlag<int> samplingRate(parser, "sampling rate", "The sampling rate to use", { 's', "sample" });
    args::ValueFlag<int> numChannels(parser, "channels", "The number of channels to use", { 'c', "channels" });
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

    RtAudio dac;
    if (dac.getDeviceCount() < 1) {
        std::cout << "No audio devices found!\n";
        return 0;
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = channels;
    parameters.firstChannel = 0;

    unsigned int bufferFrames = 256;

    PlayingContext ctx(sampleRate, channels, DlsPlayer::createFactory());
    std::cout << "Loading segment...";
    auto segment = ctx.loadSegment(args::get(segmentName));
    if (segment != nullptr) {
        std::cout << " done.\nStart playback... ";
        ctx.playSegment(*segment, SegmentTiming::Measure);
        std::cout << " done.\nBegin rendering... ";
    } else {
        std::cerr << "Cannot load segment.\n";
    }

    try {
        dac.openStream( &parameters, NULL, RTAUDIO_SINT16,
                    sampleRate, &bufferFrames, &audioCallback, (void *)&ctx );
        dac.startStream();
    } catch ( RtAudioError& e ) {
        e.printMessage();
        exit( 0 );
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
        ctx.playSegment(*segment, SegmentTiming::Measure);
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

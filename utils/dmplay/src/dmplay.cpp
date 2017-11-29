#include <iostream>
#include <queue>
#include <locale>
#include <map>
#include <dmusic/PlayingContext.h>
#include <dmusic/InstrumentPlayer.h>
#ifndef DMUSIC_TSF_SUPPORT
#define DMUSIC_TSF_SUPPORT 1
#endif
#include <dmusic/SoundFontPlayer.h>
#include <dmusic/Tracks.h>
#include <dmusic/dls/DownloadableSound.h>
#include <cassert>
#include <portaudio.h>
#include <cmath>
#include <cstdio>
#include <args.hxx>

using namespace DirectMusic;
using namespace DirectMusic::DLS;

static int paCallback(const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData) {
    /* Cast data passed through stream to our structure. */
    PlayingContext* data = (PlayingContext*)userData;
    std::int16_t *out = (std::int16_t*)outputBuffer;
    (void)inputBuffer; /* Prevent unused variable warning. */
    int samples = framesPerBuffer * data->getAudioChannels();

    data->renderBlock(out, samples, 1);
    return 0;
}

int main(int argc, char **argv) {
    args::ArgumentParser parser("dmplay plays DirectMusic segments in real time");
    args::HelpFlag help(parser, "help", "Display this help menu", { 'h', "help" });
    args::ValueFlag<int> samplingRate(parser, "sampling rate", "The sampling rate to use", { 's', "sample" });
    args::ValueFlag<int> numChannels(parser, "channels", "The number of channels to use", { 'c', "channels" });
    args::ValueFlag<std::string> sfont(parser, "soundfont", "The SoundFont directory to use during rendering", { 'f', "soundfont" });
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

    if (!sfont) {
        std::cerr << "dmplay: No soundfont directory specified." << std::endl;
        return 1;
    }

    int sampleRate = samplingRate ? args::get(samplingRate) : 44100;
    int channels = numChannels ? args::get(numChannels) : 2;

    // Store soundfonts based on their name
    std::map<std::string, tsf*> soundfontMap;
    PlayingContext ctx(sampleRate, channels, SoundFontPlayer::createMultiFactory(args::get(sfont)));
    std::cout << "Loading segment...";
    auto segment = ctx.loadSegment(args::get(segmentName));
    std::cout << " done.\nStart playback... ";
    ctx.playSegment(*segment);
    std::cout << " done.\nBegin rendering... ";

    PaStream *stream;
    PaError err;
    err = Pa_Initialize();
    if (err != paNoError) goto error;
    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream(&stream, 0, channels, paInt16, sampleRate, 256, paCallback, &ctx);
    if (err != paNoError) goto error;
    err = Pa_StartStream(stream);
    if (err != paNoError) goto error;
    std::cout << "Rendering started. Insert the next segment to be played, or 'exit' to end playback.\n";
    while (true) {
        std::string input;
        std::cin >> input;
        if (input == "exit") {
            break;
        }
        std::cout << "Loading segment...";
        segment = ctx.loadSegment(input);
        std::cout << " done.\nStart playback... ";
        ctx.playSegment(*segment);
        std::cout << " done.\nBegin rendering... ";
    }
    
    err = Pa_CloseStream(stream);
    if (err != paNoError) goto error;

    err = Pa_Terminate();
    if (err != paNoError) goto error;

    std::cout << "Rendering stopped.";
    return 0;

error:
    std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << "\n";
    return 1;
}

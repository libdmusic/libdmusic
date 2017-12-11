#include <iostream>
#include <queue>
#include <locale>
#include <map>
#include <dmusic/PlayingContext.h>
#ifndef DMUSIC_TSF_SUPPORT
#define DMUSIC_TSF_SUPPORT 1
#endif
#include <dmusic/SoundFontPlayer.h>

#ifndef DMUSIC_DLS_PLAYER
#define DMUSIC_DLS_PLAYER 1
#endif
#include <dmusic/DlsPlayer.h>
#include <dmusic/InstrumentPlayer.h>
#include <dmusic/Tracks.h>
#include <dmusic/dls/DownloadableSound.h>
#include <cassert>
#include <sndfile.h>
#include <cmath>
#include <args.hxx>

using namespace DirectMusic;
using namespace DirectMusic::DLS;

int main(int argc, char **argv) {
    args::ArgumentParser parser("dmrender renders DirectMusic segments into audio files");
    args::HelpFlag help(parser, "help", "Display this help menu", { 'h', "help" });
    args::ValueFlag<int> chunkLength(parser, "length", "The length in seconds of the audio to render", { 'l', "length" });
    args::ValueFlag<int> samplingRate(parser, "sampling rate", "The sampling rate to use", { 's', "sample" });
    args::ValueFlag<int> numChannels(parser, "channels", "The number of channels to use", { 'c', "channels" });
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
    PlayingContext ctx(sampleRate, channels, DlsPlayer::createFactory());
    std::cout << "Loading segment...";
    auto segment = ctx.loadSegment(args::get(segmentName));
    std::cout << " done.\nStart playback... ";
    ctx.playSegment(*segment);
    std::cout << " done.\nBegin rendering... \n";

    std::int16_t* buffer = new std::int16_t[length];
    for (std::uint64_t i = 0; i < length; i += sampleRate) {
        ctx.renderBlock(buffer + i, sampleRate);
        std::cout << "\rProgress: " <<  ceil((i / (float)length) * 100) << "%";
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
    delete buffer;
    std::cout << "\nRendering done.";
    return 0;
}

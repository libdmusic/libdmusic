#include <iostream>
#include <queue>
#include <locale>
#include <map>
#include <dmusic/PlayingContext.h>
#include <dmusic/SoundFontPlayer.h>
#include <dmusic/DlsPlayer.h>
#include <dmusic/InstrumentPlayer.h>
#include <dmusic/Tracks.h>
#include <dmusic/dls/DownloadableSound.h>
#include <cmath>
#include <args.hxx>

#include "../../../src/dr_wav.h"

using namespace DirectMusic;
using namespace DirectMusic::DLS;

int main(int argc, char **argv) {
    args::ArgumentParser parser("dmrender renders DirectMusic segments into audio files");
    args::HelpFlag help(parser, "help", "Display this help menu", { 'h', "help" });
    args::ValueFlag<unsigned int> chunkLength(parser, "length", "The length in seconds of the audio to render", { 'l', "length" });
    args::ValueFlag<unsigned int> samplingRate(parser, "sampling rate", "The sampling rate to use", { 's', "sample" });
    args::ValueFlag<unsigned int> numChannels(parser, "channels", "The number of channels to use", { 'c', "channels" });
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
    if(channels > 2) {
        std::cerr << "dmrender: Invalid number of channels" << std::endl;
        return 1;
    }
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

    drwav_data_format format;
    format.container = drwav_container_riff;
    format.format = DR_WAVE_FORMAT_PCM;
    format.channels = channels;
    format.sampleRate = sampleRate;
    format.bitsPerSample = 16;

    drwav* dr = drwav_open_file_write(args::get(outputFile).c_str(), &format);
    drwav_write_pcm_frames(dr, length, buffer);
    drwav_close(dr);

    delete[] buffer;

    std::cout << "\nRendering done.";
    return 0;
}

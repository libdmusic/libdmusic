#include "decode.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

#include <iostream>
#include <fstream>

using namespace DirectMusic::DLS;

std::vector<std::int16_t> decode(const Wave& sample) {
    std::vector<std::uint8_t> input = sample.getWaveFile();
    std::uint32_t channels, sampleRate;
    std::uint64_t sampleCount;

    std::int16_t* samples = drwav_open_memory_and_read_s16(input.data(), input.size(), &channels, &sampleRate, &sampleCount);

    std::vector<std::int16_t> output(samples, samples + sampleCount);
    drwav_free(samples);
    return output;
}

std::vector<float> decode_float(const Wave& sample) {
    std::vector<std::uint8_t> input = sample.getWaveFile();
    std::uint32_t channels, sampleRate;
    std::uint64_t sampleCount;

    float* samples = drwav_open_memory_and_read_f32(input.data(), input.size(), &channels, &sampleRate, &sampleCount);

    std::vector<float> output(samples, samples + sampleCount);
    drwav_free(samples);
    return output;
}
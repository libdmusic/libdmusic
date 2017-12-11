#if DMUSIC_HAS_SNDFILE

#include "decode.h"
#include <sndfile.h>
#include <iostream>
#include <fstream>

using namespace DirectMusic::DLS;

// All of this is necessary because libsndfile doesn't support
// loading files from a memory buffer directly, so we must simulate
// some sort of IO access on the membuffer.
struct Userdata {
    const std::vector<std::uint8_t> sample;
    int position;
};

static sf_count_t get_filelen(void *userdata) {
    Userdata *ud = (Userdata*)userdata;
    return ud->sample.size();
}

static sf_count_t read(void *ptr, sf_count_t count, void *userdata) {
    Userdata *ud = (Userdata*)userdata;
    uint8_t *out = (uint8_t*)ptr;
    const uint8_t *data = ud->sample.data();
    int i;
    for (i = 0; i < ud->sample.size() - ud->position && i < count; i++) {
        out[i] = data[i + ud->position];
    }
    ud->position += i;
    return i;
}

static sf_count_t tell(void *userdata) {
    Userdata *ud = (Userdata*)userdata;
    return ud->position;
}

static sf_count_t seek(sf_count_t offset, int whence, void *userdata) {
    Userdata *ud = (Userdata*)userdata;
    switch (whence) {
    case SEEK_CUR:
        ud->position += offset;
        break;
    case SEEK_END:
        ud->position = ud->sample.size() + offset;
        break;
    case SEEK_SET:
        ud->position = offset;
        break;
    }
    if (ud->position < 0) ud->position = 0;
    if (ud->position > ud->sample.size()) ud->position = ud->sample.size() - 1;
    return ud->position;
}

static sf_count_t write(const void *ptr, sf_count_t count, void *user_data) {
    return 0; // Read-only access
}

SF_VIRTUAL_IO virtio{
    get_filelen,
    seek,
    read,
    write,
    tell
};

std::vector<std::int16_t> decode(const Wave& sample) {
    SF_INFO sfinfo;
    sfinfo.channels = 0;
    sfinfo.samplerate = 0;
    sfinfo.format = 0;
    sfinfo.frames = 0;
    sfinfo.sections = 0;
    sfinfo.seekable = 0;
    std::vector<std::int16_t> out;

    std::vector<std::uint8_t> input = sample.getWaveFile();

    Userdata data { input, 0 };
    SNDFILE* file = sf_open_virtual(&virtio, SFM_READ, &sfinfo, &data);
    if (!file) return out;
    std::int16_t smp;
    sf_count_t count;
    while (count = sf_read_short(file, &smp, 1)) {
        out.push_back(smp);
    }
    sf_close(file);
    return out;
}

std::vector<float> decode_float(const Wave& sample) {
    SF_INFO sfinfo;
    sfinfo.channels = 0;
    sfinfo.samplerate = 0;
    sfinfo.format = 0;
    sfinfo.frames = 0;
    sfinfo.sections = 0;
    sfinfo.seekable = 0;
    std::vector<float> out;

    std::vector<std::uint8_t> input = sample.getWaveFile();

    Userdata data{ input, 0 };
    SNDFILE* file = sf_open_virtual(&virtio, SFM_READ, &sfinfo, &data);
    if (!file) return out;
    float smp;
    sf_count_t count;
    while (count = sf_read_float(file, &smp, 1)) {
        out.push_back(smp);
    }
    sf_close(file);
    return out;
}

#endif
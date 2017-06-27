#include "decode.h"
#include <sndfile.h>

using namespace DirectMusic::DLS;

struct Userdata {
    const Wave& sample;
    int position;
};

static sf_count_t get_filelen(void *userdata) {
    Userdata *ud = (Userdata*)userdata;
    return ud->sample.getWavedata().size();
}

static sf_count_t read(void *ptr, sf_count_t count, void *userdata) {
    Userdata *ud = (Userdata*)userdata;
    uint8_t *out = (uint8_t*)ptr;
    const uint8_t *data = ud->sample.getWavedata().data();
    int i;
    for (i = 0; i < ud->sample.getWavedata().size() - ud->position && i < count; i++) {
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
        ud->position = ud->sample.getWavedata().size() + offset;
        break;
    case SEEK_SET:
        ud->position = offset;
        break;
    }
    if (ud->position < 0) ud->position = 0;
    if (ud->position > ud->sample.getWavedata().size()) ud->position = ud->sample.getWavedata().size() - 1;
    return ud->position;
}

static sf_count_t write(const void *ptr, sf_count_t count, void *user_data) {
    return 0;
}

SF_VIRTUAL_IO virtio{
    get_filelen,
    seek,
    read,
    write,
    tell
};

static int getFormat(const WaveFormat& fmt) {
    switch (fmt.wFormatTag) {
    case WaveFormatTag::PCM:
    {
        switch (fmt.wBitsPerSample) {
        case 8:
            return SF_FORMAT_RAW | SF_FORMAT_PCM_U8;
        case 16:
            return SF_FORMAT_RAW | SF_FORMAT_PCM_16;
        }
    }
    case WaveFormatTag::ADPCM:
        return SF_FORMAT_MS_ADPCM;
    case WaveFormatTag::OKI_ADPCM:
        return SF_FORMAT_IMA_ADPCM;
    case WaveFormatTag::IEEE_FLOAT:
    {
        switch (fmt.wBitsPerSample) {
        case 32:
            return SF_FORMAT_WAV | SF_FORMAT_FLOAT;
        case 64:
            return SF_FORMAT_WAV | SF_FORMAT_DOUBLE;
        }
    }
    case WaveFormatTag::MULAW:
        return SF_FORMAT_WAV | SF_FORMAT_ULAW;
    case WaveFormatTag::ALAW:
        return SF_FORMAT_WAV | SF_FORMAT_ALAW;
    case WaveFormatTag::GSM610:
        return SF_FORMAT_WAV | SF_FORMAT_GSM610;
    case WaveFormatTag::G721_ADPCM:
        return SF_FORMAT_WAV | SF_FORMAT_G721_32;
    }
    return 0;
}

std::vector<std::int16_t> decode(const Wave& sample) {
    SF_INFO sfinfo;
    sfinfo.channels = sample.getWaveformat().wChannels;
    sfinfo.samplerate = sample.getWaveformat().dwSamplesPerSec;
    sfinfo.format = getFormat(sample.getWaveformat());
    sfinfo.frames = 0;
    sfinfo.sections = 0;
    sfinfo.seekable = 0;
    std::vector<std::int16_t> out;
    if (sfinfo.format == 0) {
        return out;
    }

    Userdata data {
        sample,
        0
    };
    SNDFILE* file = sf_open_virtual(&virtio, SFM_READ, &sfinfo, &data);
    if (!file) {
        std::string err(sf_strerror(file));
    }
    std::int16_t smp;
    sf_count_t count;
    while (count = sf_read_short(file, &smp, 1)) {
        out.push_back(smp);
    }
    //sf_close(file);
    return out;
}
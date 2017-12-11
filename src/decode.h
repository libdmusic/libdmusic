#pragma once

#if !DMUSIC_HAS_SNDFILE
#error "libsndfile is needed"
#endif

#include <cstdint>
#include <vector>
#include <dmusic/dls/DownloadableSound.h>

std::vector<std::int16_t> decode(const DirectMusic::DLS::Wave& sample);
std::vector<float> decode_float(const DirectMusic::DLS::Wave& sample);
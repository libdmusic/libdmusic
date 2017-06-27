#pragma once
#include <cstdint>
#include <vector>
#include <dmusic/dls/DownloadableSound.h>

std::vector<std::int16_t> decode(const DirectMusic::DLS::Wave& sample);
#pragma once

#include <cstdint>
#include <vector>
#include "Common.h"
#include "Track.h"
#include "Riff.h"

namespace DirectMusic {
    class Segment {
    public:
        Segment(DirectMusic::Riff::Chunk& c);
    private:
        DMUS_IO_SEGMENT_HEADER m_header;
        std::vector<Track> m_tracks;
    };
}

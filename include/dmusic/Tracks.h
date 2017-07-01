#pragma once

#include "Forms.h"

namespace DirectMusic {
    class BandTrackForm
        : public SubtrackForm {};

    class ChordTrack
        : public SubtrackForm {};

    class ChordmapTrack
        : public SubtrackForm {};

    class CommandTrack
        : public SubtrackForm {};

    class LyricsTrack
        : public SubtrackForm {};

    class MarkerTrack
        : public SubtrackForm {};

    class MuteTrack
        : public SubtrackForm {};

    class ParameterControlTrack
        : public SubtrackForm {};

    class PatternTrack
        : public SubtrackForm {};

    class ScriptTrack
        : public SubtrackForm {};

    class SegmentTriggerTrack
        : public SubtrackForm {};

    class SequenceTrack
        : public SubtrackForm {};

    class SignpostTrack
        : public SubtrackForm {};

    class StyleTrack
        : public SubtrackForm {};

    class SysexTrack
        : public SubtrackForm {};

    class TempoTrack
        : public SubtrackForm {};

    class TimeSignatureTrack
        : public SubtrackForm {};

    class WaveTrack
        : public SubtrackForm {};
}
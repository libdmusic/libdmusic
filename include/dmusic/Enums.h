#pragma once

#include <cstdint>

namespace DirectMusic {
    /// The DMUS_CHORDKEYF_FLAGS enumerated type is used in the bFlags member of the DMUS_CHORD_KEY structure.
    enum DMUS_CHORDKEYF_FLAGS {
        /// The chord is silent.
        DMUS_CHORDKEYF_SILENT = 1
    };

    /// The DMUS_CLOCKTYPE enumerated type is used in the ctType member of the DMUS_CLOCKINFO8 structure.
    enum DMUS_CLOCKTYPE {
        /// Clock is the system clock.
        DMUS_CLOCK_SYSTEM = 0,

        /// Clock is on a waveform-playback device.
        DMUS_CLOCK_WAVE = 1
    };

    /// The DMUS_COMMANDT_TYPES enumerated type is used in the bCommand member of the DMUS_COMMAND_PARAM structure.
    enum class DMUS_COMMANDT_TYPES : std::uint8_t {
        /// The command is a groove command.
        DMUS_COMMANDT_GROOVE = 0,

        /// The command is a fill.
        DMUS_COMMANDT_FILL = 1,

        /// The command is an introduction.
        DMUS_COMMANDT_INTRO = 2,

        /// The command is a break.
        DMUS_COMMANDT_BREAK = 3,

        /// The command is an ending.
        DMUS_COMMANDT_END = 4,

        /// The command is an ending and an introduction.
        DMUS_COMMANDT_ENDANDINTRO = 5
    };

    enum DMUS_COMPOSEF_FLAGS {
        /// No flags. By default, the transition starts on a measure boundary.
        DMUS_COMPOSEF_NONE = 0,

        /// Align transition to the time signature of the currently playing segment.
        DMUS_COMPOSEF_ALIGN = 0x1,

        /// Overlap the transition into pToSeg. Not implemented.
        DMUS_COMPOSEF_OVERLAP = 0x2,

        /// AutoTransition only. Start transition immediately.
        DMUS_COMPOSEF_IMMEDIATE = 0x4,

        /// AutoTransition only. Start transition on a grid boundary.
        DMUS_COMPOSEF_GRID = 0x8,

        /// AutoTransition only. Start transition on a beat boundary.
        DMUS_COMPOSEF_BEAT = 0x10,

        /// AutoTransition only. Start transition on a measure boundary.
        DMUS_COMPOSEF_MEASURE = 0x20,

        /// AutoTransition only.
        /// Use the DMUS_SEGF_AFTERPREPARETIME flag when cueing the transition.
        DMUS_COMPOSEF_AFTERPREPARETIME = 0x40,

        /// Allow the switch to occur on any beat.
        /// Used in conjunction with DMUS_COMPOSEF_ALIGN.
        DMUS_COMPOSEF_VALID_START_BEAT = 0x80,

        /// Allow the switch to occur on any grid.
        /// Used in conjunction with DMUS_COMPOSEF_ALIGN.
        DMUS_COMPOSEF_VALID_START_GRID = 0x100,

        /// Allow the switch to occur at any time.
        /// Used in conjunction with DMUS_COMPOSEF_ALIGN.
        DMUS_COMPOSEF_VALID_START_TICK = 0x200,

        /// Play the transition at the end of the current segment.
        DMUS_COMPOSEF_SEGMENTEND = 0x400,

        /// Play the transition at the next marker in the current segment.
        DMUS_COMPOSEF_MARKER = 0x800,

        /// Compose a transition that modulates smoothly from pFromSeg to pToSeg, using the chord of pToSeg.
        DMUS_COMPOSEF_MODULATE = 0x1000,

        /// Composes a long transition.
        /// If this flag is not set, the length of the transition is at most one measure unless the wCommand parameter of
        /// ComposeTransition or AutoTransition specifies an ending and the style contains an ending of greater than one measure.
        /// If this flag is set, the length of the transition increases by one measure.
        DMUS_COMPOSEF_LONG = 0x2000,

        /// Include the entire transition pattern.
        DMUS_COMPOSEF_ENTIRE_TRANSITION = 0x4000,

        /// Include one bar of the transition pattern.
        DMUS_COMPOSEF_1BAR_TRANSITION = 0x8000,

        /// Include the additional transition pattern in its entirety.
        /// Used in combination with DMUS_COMPOSEF_LONG.
        DMUS_COMPOSEF_ENTIRE_ADDITION = 0x10000,

        /// Include one bar of the additional transition pattern.
        /// This is the default behavior when DMUS_COMPOSEF_LONG is specified.
        DMUS_COMPOSEF_1BAR_ADDITION = 0x20000,

        /// Allow the switch to occur on any bar.  Used in combination with DMUS_COMPOSEF_ALIGN.
        DMUS_COMPOSEF_VALID_START_MEASURE = 0x40000,

        /// Use the segment's default boundary.
        DMUS_COMPOSEF_DEFAULT = 0x80000,

        /// Do not invalidate segments that are playing.
        DMUS_COMPOSEF_NOINVALIDATE = 0x100000,

        /// Use the audiopaths embedded in the segments.
        DMUS_COMPOSEF_USE_AUDIOPATH = 0x200000,

        /// Invalidate only the primary segment when transitioning to a new segment.
        DMUS_COMPOSEF_INVALIDATE_PRI = 0x400000
    };

    /// The DMUS_CURVE_FLAGS enumerated type is used in the bFlags member of the DMUS_CURVE_PMSG structure.
    enum DMUS_CURVE_FLAGS {
        /// The value of DMUS_CURVE_PMSG.nResetValue must be set when the time is reached
        /// or an invalidation occurs because of a transition.
        /// If this flag is not set, the curve stays permanently at the new value.
        DMUS_CURVE_RESET = 1,

        /// Ignore DMUS_CURVE_PMSG.nStartValue and start the curve at the current value.
        /// This works only for volume, expression, and pitch bend.
        DMUS_CURVE_START_FROM_CURRENT = 2
    };

    enum DMUS_CURVE_TYPE {
        DMUS_CURVET_PBCURVE = 0x03,   /* Pitch bend curve. */
        DMUS_CURVET_CCCURVE = 0x04,   /* Control change curve. */
        DMUS_CURVET_MATCURVE = 0x05,   /* Mono aftertouch curve. */
        DMUS_CURVET_PATCURVE = 0x06,   /* Poly aftertouch curve. */
        DMUS_CURVET_RPNCURVE = 0x07,   /* RPN curve with curve type in wParamType. */
        DMUS_CURVET_NRPNCURVE = 0x08   /* NRPN curve with curve type in wParamType. */
    };

    enum DMUS_CURVE_SHAPE {
        DMUS_CURVES_LINEAR = 0,
        DMUS_CURVES_INSTANT = 1,
        DMUS_CURVES_EXP = 2,
        DMUS_CURVES_LOG = 3,
        DMUS_CURVES_SINE = 4
    };

    /// The DMUS_EMBELLISHMENT_TYPES enumerated type is used in the wEmbellishment member of the DMUS_IO_PATTERN structure.
    enum DMUS_EMBELLISHT_TYPES {
        /// Normal pattern.
        DMUS_EMBELLISHT_NORMAL = 0,

        /// Fill pattern.
        DMUS_EMBELLISHT_FILL = 1,

        /// Break pattern.
        DMUS_EMBELLISHT_BREAK = 2,
        
        /// Intro pattern.
        DMUS_EMBELLISHT_INTRO = 4,
        
        /// End pattern.
        DMUS_EMBELLISHT_END = 8,
        
        /// Motif pattern.
        DMUS_EMBELLISHT_MOTIF = 16,

        /// Combination of all types
        DMUS_EMBELLISHT_ALL = 0xFFFF
    };

    /// The DMUS_NOTEF_FLAGS enumerated type is used in the bFlags member of the DMUS_NOTE_PMSG structure.
    enum DMUS_NOTEF_FLAGS {
        /// MIDI note on.
        DMUS_NOTEF_NOTEON = 1,

        /// Do not invalidate the note.
        DMUS_NOTEF_NOINVALIDATE = 2,

        /// Do not invalidate if the note is still within the scale.
        DMUS_NOTEF_NOINVALIDATE_INSCALE = 4,

        /// Do not invalidate if the note is still within the chord.
        DMUS_NOTEF_NOINVALIDATE_INCHORD = 8,

        /// Regenerate the note when a chord change occurs.
        /// The note's music value, subchord level, and play mode flags are used to construct
        /// a new note according to the new chord and scale.
        /// If the original note had a timing offset, this is applied to the start time of the new note.
        DMUS_NOTEF_REGENERATE = 0x10
    };

    /// The DMUS_PATTERNT_TYPES enumerated type is used in various command structures
    /// to control the way patterns are selected in sequential commands.
    enum DMUS_PATTERNT_TYPES {
        /// Select a random matching pattern.
        DMUS_PATTERNT_RANDOM = 0,

        /// Repeat the last matching pattern.
        DMUS_PATTERNT_REPEAT = 1,

        /// Play matching patterns sequentially, in the order loaded, starting with the first.
        DMUS_PATTERNT_SEQUENTIAL = 2,

        /// Play matching patterns sequentially, in the order loaded, starting at a random point in the sequence.
        DMUS_PATTERNT_RANDOM_START = 3,

        /// Play randomly, but do not play the same pattern twice.
        DMUS_PATTERNT_NO_REPEAT = 4,

        /// Play randomly, but do not repeat any pattern until all have played.
        DMUS_PATTERNT_RANDOM_ROW = 5
    };

    /// The DMUS_PLAYMODE_FLAGS enumerated type is used in various structures to specify play modes.
    /// The play mode determines how a music value is transposed to a MIDI note. 
    enum DMUS_PLAYMODE_FLAGS {
        /// Interpret the music value as a MIDI value.
        /// This is defined as 0 and signifies the absence of other flags.
        /// This flag is used for drums, sound effects, and sequenced notes
        /// that should not be transposed by the chord or scale.
        DMUS_PLAYMODE_FIXED = 0,

        /// Transpose over the key root.
        DMUS_PLAYMODE_KEY_ROOT = 1,

        /// Transpose over the chord root.
        DMUS_PLAYMODE_CHORD_ROOT = 2,

        /// Use scale intervals from a scale pattern.
        DMUS_PLAYMODE_SCALE_INTERVALS = 4,

        /// Use chord intervals from a chord pattern.
        DMUS_PLAYMODE_CHORD_INTERVALS = 8,

        /// No mode. Indicates that the parent part's mode should be used.
        DMUS_PLAYMODE_NONE = 16,
    };

    /// The DMUS_PMSGF_FLAGS enumerated type is used in the dwFlags member of the DMUS_PMSG structure.
    enum DMUS_PMSGF_FLAGS {
        /// The rtTime member is valid.
        DMUS_PMSGF_REFTIME = 1,

        /// The mtTime member is valid.
        DMUS_PMSGF_MUSICTIME = 2,

        /// Message should be processed immediately, regardless of its time stamp.
        DMUS_PMSGF_TOOL_IMMEDIATE = 4,

        /// Message should be processed just before its time stamp, allowing for port latency.
        DMUS_PMSGF_TOOL_QUEUE = 8,

        /// Message should be processed at the time stamp.
        DMUS_PMSGF_TOOL_ATTIME = 0x10,

        /// Message is being flushed.
        DMUS_PMSGF_TOOL_FLUSH = 0x20,

        /// Value in rtTime cannot be overridden by a tempo change.
        DMUS_PMSGF_LOCKTOREFTIME = 0x40,

        /// Message has valid members not present in versions prior to DirectX 8.0.
        DMUS_PMSGF_DX8 = 0x80
    };

    /// The DMUS_PMSGT_TYPES enumerated type is used in the dwType member of the DMUS_PMSG structure to identify the type of message.
    enum DMUS_PMSGT_TYPES {
        /// MIDI channel message. See DMUS_MIDI_PMSG.
        DMUS_PMSGT_MIDI = 0,

        /// Music note. See DMUS_NOTE_PMSG.
        DMUS_PMSGT_NOTE = 1,

        /// MIDI system exclusive message. See DMUS_SYSEX_PMSG.
        DMUS_PMSGT_SYSEX = 2,

        /// Notification message. See DMUS_NOTIFICATION_PMSG.
        DMUS_PMSGT_NOTIFICATION = 3,

        /// Tempo message. See DMUS_TEMPO_PMSG.
        DMUS_PMSGT_TEMPO = 4,

        /// Control change and pitch-bend curve. See DMUS_CURVE_PMSG.
        DMUS_PMSGT_CURVE = 5,

        /// Time signature. See DMUS_TIMESIG_PMSG.
        DMUS_PMSGT_TIMESIG = 6,

        /// Patch change. See DMUS_PATCH_PMSG.
        DMUS_PMSGT_PATCH = 7,

        /// Transposition. See DMUS_TRANSPOSE_PMSG.
        DMUS_PMSGT_TRANSPOSE = 8,

        /// Channel priority change. See DMUS_CHANNEL_PRIORITY_PMSG.
        DMUS_PMSGT_CHANNEL_PRIORITY = 9,

        /// Stop message. See DMUS_PMSG.
        DMUS_PMSGT_STOP = 10,

        /// A control segment has started or ended. See DMUS_PMSG.
        DMUS_PMSGT_DIRTY = 11,

        /// Control information for playing a waveform. See DMUS_WAVE_PMSG.
        DMUS_PMSGT_WAVE = 12,

        /// Lyric message. See DMUS_LYRIC_PMSG.
        DMUS_PMSGT_LYRIC = 13,

        /// Lyric message sent by a script. See DMUS_LYRIC_PMSG.
        DMUS_PMSGT_SCRIPTLYRIC = 14,

        /// User-defined message.
        DMUS_PMSGT_USER = 255
    };

    enum DMUS_SEGF_FLAGS : std::uint32_t {
        /// Time parameter is in reference time.
        DMUS_SEGF_REFTIME = 1 << 6,

        /// Secondary segment.
        DMUS_SEGF_SECONDARY = 1 << 7,

        /// For a primary segment, play at the end of the primary segment queue.
        DMUS_SEGF_QUEUE = 1 << 8,

        /// Play as a control segment. Valid for secondary segments only.
        DMUS_SEGF_CONTROL = 1 << 9,

        /// Resolve time to a time after the prepare time.
        DMUS_SEGF_AFTERPREPARETIME = 1 << 10,

        /// Resolve time to a grid boundary.
        DMUS_SEGF_GRID = 1 << 11,

        /// Resolve time to a beat boundary.
        DMUS_SEGF_BEAT = 1 << 12,

        /// Resolve time to a measure boundary.
        DMUS_SEGF_MEASURE = 1 << 13,

        /// Use flags embedded in the segment.
        /// This resolves the time to the segment's default boundary and also causes
        /// the segment to play on its embedded audiopath, if it was configured to do
        /// so in the authoring application.
        DMUS_SEGF_DEFAULT = 1 << 14,

        /// Setting this flag for a primary or control segment causes the new segment not to cause an invalidation.
        /// Without this flag, an invalidation occurs, cutting off and resetting any currently playing curve or note.
        /// This flag should be combined with DMUS_SEGF_AFTERPREPARETIME so that notes in the new segment do not play over notes played by the old segment.
        DMUS_SEGF_NOINVALIDATE = 1 << 15,

        /// The beginning of the segment can be aligned with a boundary, such as measure or beat, that has already passed.
        /// For this to happen, the segment must have a valid start point that falls before the next boundary.
        /// Start points can be defined in the segment, or one of the DMUS_SEGF_VALID_START_* flags can be used to define
        /// the granularity of valid start points. Any DMUS_SEGF_VALID_START_* flag takes effect only if a valid start point is not defined in the segment.
        DMUS_SEGF_ALIGN = 1 << 16,

        /// Allow the start to occur on any beat. Used in combination with DMUS_SEGF_ALIGN.
        DMUS_SEGF_VALID_START_BEAT = 1 << 17,

        /// Allow the start to occur on any grid. Used in combination with DMUS_SEGF_ALIGN.
        DMUS_SEGF_VALID_START_GRID = 1 << 18,

        /// Allow the start to occur at any time. Used in combination with DMUS_SEGF_ALIGN.
        DMUS_SEGF_VALID_START_TICK = 1 << 19,

        /// Compose and play a transition segment, using the transition template.
        DMUS_SEGF_AUTOTRANSITION = 1 << 20,

        /// Resolve time to a time after the queue time.
        /// This is the default for primary segments.
        /// Ignored if DMUS_SEGF_AFTERPREPARETIME is also set.
        DMUS_SEGF_AFTERQUEUETIME = 1 << 21,

        /// Resolve time to a time after the latency time.
        /// This is true for all segments, so this flag currently has no effect.
        DMUS_SEGF_AFTERLATENCYTIME = 1 << 22,

        /// Play at the end of the primary segment that is playing at the start time.
        /// If the new segment is being played as a primary segment, any primary segments
        /// already queued after the currently playing primary segment are flushed.
        /// If no primary segment is playing, use other resolution flags.
        /// When combined with DMUS_SEGF_ALIGN, this flag causes the beginning of the
        /// cued segment to be aligned with the beginning of the current primary segment.
        DMUS_SEGF_SEGMENTEND = 1 << 23,

        /// Resolve time to the next marker in the primary segment.
        /// If there are no markers, use other resolution flags.
        DMUS_SEGF_MARKER = 1 << 24,

        /// Align start time with current time signature, even if there is no primary segment.
        DMUS_SEGF_TIMESIG_ALWAYS = 1 << 25,

        /// Use the audiopath embedded in the segment.
        /// Automatic downloading of bands must be enabled to ensure that the segment plays correctly.
        DMUS_SEGF_USE_AUDIOPATH = 1 << 26,

        /// Allow the start to occur at the beginning of a measure.
        /// Used in combination with DMUS_SEGF_ALIGN.
        DMUS_SEGF_VALID_START_MEASURE = 1 << 27,

        /// Invalidate only the primary segment when transitioning to a new segment.s
        DMUS_SEGF_INVALIDATE_PRI = 1 << 28
    };
    
    /// The DMUS_TIME_RESOLVE_FLAGS enumerated type is used in the dwFlags member of the DMUS_PMSG structure.
    enum DMUS_TIME_RESOLVE_FLAGS {
        /// Resolve to a time after the prepare time.
        DMUS_TIME_RESOLVE_AFTERPREPARETIME = DMUS_SEGF_AFTERPREPARETIME,

        /// Resolve to a time after the queue time.
        DMUS_TIME_RESOLVE_AFTERQUEUETIME = DMUS_SEGF_AFTERQUEUETIME,

        /// Resolve to a time after the latency time.
        DMUS_TIME_RESOLVE_AFTERLATENCYTIME = DMUS_SEGF_AFTERLATENCYTIME,

        /// Resolve to a time on a grid boundary.
        DMUS_TIME_RESOLVE_GRID = DMUS_SEGF_GRID,

        /// Resolve to a time on a beat boundary.
        DMUS_TIME_RESOLVE_BEAT = DMUS_SEGF_BEAT,

        /// Resolve to a time on a measure boundary.
        DMUS_TIME_RESOLVE_MEASURE = DMUS_SEGF_MEASURE,

        /// Resolve to a marker.
        DMUS_TIME_RESOLVE_MARKER = DMUS_SEGF_MARKER,

        /// Resolve to the end of the segment.
        DMUS_TIME_RESOLVE_SEGMENTEND = DMUS_SEGF_SEGMENTEND
    };

    /// The DMUS_VARIATIONT_TYPES enumerated type is used in the DMUS_IO_PARTREF structure
    /// to specify the way variations are selected in sequential commands.
    enum DMUS_VARIATIONT_TYPES {
        /// Play matching variations sequentially, in the order loaded, starting with the first.
        DMUS_VARIATIONT_SEQUENTIAL = 0,

        /// Select a random matching variation.
        /// This is the behavior in versions prior to DirectX 8.0.
        DMUS_VARIATIONT_RANDOM = 1,

        /// Play matching variations sequentially, in the order loaded, starting at a random point in the sequence.
        DMUS_VARIATIONT_RANDOM_START = 2,

        /// Play randomly, but do not play the same variation twice.
        DMUS_VARIATIONT_NO_REPEAT = 3,

        /// Play randomly, but do not repeat any variation until all have played.
        DMUS_VARIATIONT_RANDOM_ROW = 4
    };
}

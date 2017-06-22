#pragma once

#include <cstdint>
#include "Guid.h"

namespace DirectMusic {
    #pragma pack(push, 1)

    // The DMUS_IO_BAND_ITEM_HEADER structure contains information about a band change.
    // Used in the Band Track Form of older files.
    // It has been superseded by DMUS_IO_BAND_ITEM_HEADER2
    struct DMUS_IO_BAND_ITEM_HEADER {
        // Time of the band change.
        std::uint32_t lBandTime;
    };

    // The DMUS_IO_BAND_ITEM_HEADER2 structure contains information about a band change.
    // Used in the Band Track Form.
    struct DMUS_IO_BAND_ITEM_HEADER2 {
        // Time in the music with which the band change is associated.
        std::uint32_t lBandTimeLogical;

        // Precise time when band change will take effect. Should be close to logical time.
        std::uint32_t lBandTimePhysical;
    };

    // The DMUS_IO_BAND_TRACK_HEADER structure contains information about the default behavior of a band track.
    // Used in the Band Track Form.
    struct DMUS_IO_BAND_TRACK_HEADER {
        // Flag for automatic downloading of instruments when a segment is played.
        std::uint32_t bAutoDownload;
    };

    // The DMUS_IO_BUFFER_ATTRIBUTES_HEADER structure describes attributes of a DirectSound buffer.
    // Used in the Audiopath Form.
    struct DMUS_IO_BUFFER_ATTRIBUTES_HEADER {
        // Unique identifier of the buffer configuration.
        GUID guidBufferID;

        // Flags describing the buffer.
        std::uint32_t dwFlags;
    };

    // The DMUS_IO_CHORD structure contains information about a chord change.
    // Used in the Chord Track List.
    struct DMUS_IO_CHORD {
        // Name of the chord.
        char wszName[16];

        // Time of the chord.
        std::uint32_t mtTime;

        // Measure that the chord falls on.
        std::uint16_t wMeasure;

        // Beat that the chord falls on.
        std::uint8_t bBeat;

        std::uint8_t bFlags;
    };

    // The DMUS_IO_CHORDENTRY structure contains information about a chord entry.
    // Used in the Chordmap Form.
    struct DMUS_IO_CHORDENTRY {
        // Flag indicating whether the chord is a starting chord (bit 2 set)
        //  or an ending chord (bit 3 set) in the chord graph.
        std::uint32_t dwFlags;

        // Replaces the run-time pointer to this.
        // Each chord entry is tagged with a unique connection identifier.
        std::uint16_t wConnectionID;
    };

    // The DMUS_IO_CHORDMAP structure contains information about a chordmap.
    // Used in the Chordmap Form.
    struct DMUS_IO_CHORDMAP {
        // Name of the chordmap, used in the object description when the chordmap is loaded.
        char wszLoadName[20];

        // Scale associated with the chordmap.
        // Each of the lower 24 bits represents a semitone,
        //  starting with the root at the least significant bit,
        //  and the bit is set if the note is in the scale.
        std::uint32_t dwScalePattern;

        std::uint32_t dwFlags;
    };

    // The DMUS_IO_CHORDMAP_SIGNPOST structure contains information about a signpost chord in a chordmap.
    // Used in the Chordmap Form.
    struct DMUS_IO_CHORDMAP_SIGNPOST {
        // Types of signpost supported by this chord.
        // The values are used to match against the same values as they appear in templates.
        // Composing from template consists of (among other things) looking for these values
        //  in the template and finding actual chords in the chordmap that match these values.
        std::uint32_t dwChords;

        // Flags defining whether this chord is to be preceded by cadence chords.
        // Signpost chords can have up to two cadence chords.
        // This value can be SPOST_CADENCE1 (first cadence), SPOST_CADENCE2 (second cadence), or
        //  a combination of these two flags.
        std::uint32_t dwFlags;
    };

    // The DMUS_IO_CHORDMAP_SUBCHORD structure contains information about a subchord.
    // Used in the Chordmap Form.
    struct DMUS_IO_CHORDMAP_SUBCHORD {
        // Notes in the subchord.
        // Each of the lower 24 bits represents a semitone,
        //  starting with the root at the least significant bit,
        //  and the bit is set if the note is in the chord.
        std::uint32_t dwChordPattern;

        // Notes in the scale.
        // Each of the lower 24 bits represents a semitone,
        //  starting with the root at the least significant bit,
        //  and the bit is set if the note is in the scale.
        std::uint32_t dwScalePattern;

        // Points in the scale at which inversions can occur.
        // Bits that are off signify that the notes in the interval cannot be inverted.
        // Thus, the pattern 100001111111 indicates that inversions are allowed anywhere
        //  except between the fifth and seventh degrees of a major scale.
        std::uint32_t dwInvertPattern;

        // Root of the subchord, where 0 is the lowest C in the range and 23 is the top B.
        std::uint8_t bChordRoot;

        // Root of the scale, where 0 is the lowest C in the range and 23 is the top B.
        std::uint8_t bScaleRoot;

        // Reserved for future use.
        std::uint16_t wCFlags;

        // Bit field showing which levels are supported by this subchord.
        // Each part in a style is assigned a level, and this chord is used
        //  only for parts whose levels are contained in this member.
        std::uint32_t dwLevels;
    };


    // The DMUS_IO_COMMAND structure contains information about a command event.
    // Used in the Command Track Chunk.
    struct DMUS_IO_COMMAND {
        // Time of the command.
        std::uint32_t mtTime;

        // Measure that the command falls on.
        std::uint16_t wMeasure;

        // Beat that the command falls on.
        std::uint8_t bBeat;

        // Command type. See DMUS_COMMANDT_TYPES.
        std::uint8_t bCommand;

        // Groove level, or 0 if the command is not a groove command.
        std::uint8_t bGrooveLevel;

        // Size of the range within which the groove level can be randomized.
        // If this value is an odd number, the groove range is bGrooveRange � 1.
        // For instance, if the groove level is 35 and bGrooveRange is 5,
        //  the adjusted groove range is 4 and the groove level could be anywhere from 33 to 37.
        std::uint8_t bGrooveRange;

        // Flag that specifies how patterns are selected for repetition. See DMUS_PATTERNT_TYPES.
        std::uint8_t bRepeatMode;
    };

    // The DMUS_IO_CONTAINED_OBJECT_HEADER structure is used before each object in a Container Form.
    struct DMUS_IO_CONTAINED_OBJECT_HEADER {
        // Class identifier of the object.
        GUID guidClassID;

        std::uint32_t dwFlags;

        // Identifier of the data chunk.
        // If this value is zero, it is assumed that the chunk is of type LIST,
        //  so fccType is valid and must be nonzero.
        char ckid[4];

        // List type.
        // If this value is zero, ckid is valid and must be nonzero.
        char fccType[4];
    };

    // The DMUS_IO_CONTAINER_HEADER structure is used in the Container Form.
    struct DMUS_IO_CONTAINER_HEADER {
        std::uint32_t dwFlags;
    };

    struct DMUS_IO_VERSION {
        std::uint32_t dwVersionMS;
        std::uint32_t dwVersionLS;
    };

    struct DMUS_IO_SEGMENT_HEADER {
        std::uint32_t dwRepeats;
        std::uint32_t mtLength;
        std::uint32_t mtPlayStart;
        std::uint32_t mtLoopStart;
        std::uint32_t mtLoopEnd;
        std::uint32_t dwResolution;
        std::uint64_t rtLength;
        std::uint32_t dwFlags;
        std::uint32_t dwReserved;
        std::uint64_t rtLoopStart;
        std::uint64_t rtLoopEnd;
        std::uint64_t rtPlayStart;
    };

    struct DMUS_IO_TRACK_HEADER {
        GUID guidClassID;
        std::uint32_t dwPosition;
        std::uint32_t dwGroup;
        char ckid[4];
        char fccType[4];
    };

    struct DMUS_IO_TRACK_EXTRAS_HEADER {
        std::uint32_t dwFlags;
        std::uint32_t dwPriority;
    };

    enum DMUS_SEGF_FLAGS {
        DMUS_SEGF_REFTIME = 1 << 6,
        DMUS_SEGF_SECONDARY = 1 << 7,
        DMUS_SEGF_QUEUE = 1 << 8,
        DMUS_SEGF_CONTROL = 1 << 9,
        DMUS_SEGF_AFTERPREPARETIME = 1 << 10,
        DMUS_SEGF_GRID = 1 << 11,
        DMUS_SEGF_BEAT = 1 << 12,
        DMUS_SEGF_MEASURE = 1 << 13,
        DMUS_SEGF_DEFAULT = 1 << 14,
        DMUS_SEGF_NOINVALIDATE = 1 << 15,
        DMUS_SEGF_ALIGN = 1 << 16,
        DMUS_SEGF_VALID_START_BEAT = 1 << 17,
        DMUS_SEGF_VALID_START_GRID = 1 << 18,
        DMUS_SEGF_VALID_START_TICK = 1 << 19,
        DMUS_SEGF_AUTOTRANSITION = 1 << 20,
        DMUS_SEGF_AFTERQUEUETIME = 1 << 21,
        DMUS_SEGF_AFTERLATENCYTIME = 1 << 22,
        DMUS_SEGF_SEGMENTEND = 1 << 23,
        DMUS_SEGF_MARKER = 1 << 24,
        DMUS_SEGF_TIMESIG_ALWAYS = 1 << 25,
        DMUS_SEGF_USE_AUDIOPATH = 1 << 26,
        DMUS_SEGF_VALID_START_MEASURE = 1 << 27,
        DMUS_SEGF_INVALIDATE_PRI = 1 << 28
    };

    enum DMUS_TIME_RESOLVE_FLAGS {
        DMUS_TIME_RESOLVE_AFTERPREPARETIME = DMUS_SEGF_AFTERPREPARETIME,
        DMUS_TIME_RESOLVE_AFTERQUEUETIME = DMUS_SEGF_AFTERQUEUETIME,
        DMUS_TIME_RESOLVE_AFTERLATENCYTIME = DMUS_SEGF_AFTERLATENCYTIME,
        DMUS_TIME_RESOLVE_GRID = DMUS_SEGF_GRID,
        DMUS_TIME_RESOLVE_BEAT = DMUS_SEGF_BEAT,
        DMUS_TIME_RESOLVE_MEASURE = DMUS_SEGF_MEASURE,
        DMUS_TIME_RESOLVE_MARKER = DMUS_SEGF_MARKER,
        DMUS_TIME_RESOLVE_SEGMENTEND = DMUS_SEGF_SEGMENTEND
    };

    #pragma pack(pop)
}

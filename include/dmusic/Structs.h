#pragma once

#include <cstdint>
#include <array>
#include "Common.h"
#include "Enums.h"

namespace DirectMusic {
#pragma pack(push, 1)
    /// The DMUS_IO_TIMESIG structure contains information about the time signature of a segment.
    /// Used in the DMUS_IO_STYLE, DMUS_IO_VERSION, and DMUS_IO_PATTERN structures.
    struct DMUS_IO_TIMESIG {
        /// Beats per measure (top of time signature).
        std::uint8_t bBeatsPerMeasure;

        /// Note that receives the beat (bottom of the time signature),
        /// where 1 is a whole note, 2 is a half note, 4 is a quarter note, and so on.
        /// Zero is a 256th note.
        std::uint8_t bBeat;

        /// Grids (subdivisions) per beat.
        std::uint16_t wGridsPerBeat;

        bool operator==(const DMUS_IO_TIMESIG& a) const {
            return a.bBeat == bBeat &&
                a.bBeatsPerMeasure == bBeatsPerMeasure &&
                a.wGridsPerBeat == wGridsPerBeat;
        }

        DMUS_IO_TIMESIG() {}
        DMUS_IO_TIMESIG(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_TIMESIG, bBeatsPerMeasure, std::uint8_t);
            FIELDINIT(DMUS_IO_TIMESIG, bBeat, std::uint8_t);
            FIELDINIT(DMUS_IO_TIMESIG, wGridsPerBeat, std::uint16_t);
        }
    };

    /// The DMUS_IO_BAND_ITEM_HEADER structure contains information about a band change.
    /// Used in the Band Track Form of older files.
    /// It has been superseded by DMUS_IO_BAND_ITEM_HEADER2
    struct DMUS_IO_BAND_ITEM_HEADER {
        /// Time of the band change.
        std::uint32_t lBandTime;

        DMUS_IO_BAND_ITEM_HEADER() {}
        DMUS_IO_BAND_ITEM_HEADER(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_BAND_ITEM_HEADER, lBandTime, std::uint32_t);
        }
    };

    /// The DMUS_IO_BAND_ITEM_HEADER2 structure contains information about a band change.
    /// Used in the Band Track Form.
    struct DMUS_IO_BAND_ITEM_HEADER2 {
        /// Time in the music with which the band change is associated.
        std::uint32_t lBandTimeLogical;

        /// Precise time when band change will take effect. Should be close to logical time.
        std::uint32_t lBandTimePhysical;

        DMUS_IO_BAND_ITEM_HEADER2() {}
        DMUS_IO_BAND_ITEM_HEADER2(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_BAND_ITEM_HEADER2, lBandTimeLogical, std::uint32_t);
            FIELDINIT(DMUS_IO_BAND_ITEM_HEADER2, lBandTimePhysical, std::uint32_t);
        }
    };

    /// The DMUS_IO_BAND_TRACK_HEADER structure contains information about the default behavior of a band track.
    /// Used in the Band Track Form.
    struct DMUS_IO_BAND_TRACK_HEADER {
        /// Flag for automatic downloading of instruments when a segment is played.
        std::uint32_t bAutoDownload;

        DMUS_IO_BAND_TRACK_HEADER() {}
        DMUS_IO_BAND_TRACK_HEADER(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_BAND_TRACK_HEADER, bAutoDownload, std::uint32_t);
        }
    };

    /// The DMUS_IO_BUFFER_ATTRIBUTES_HEADER structure describes attributes of a DirectSound buffer.
    /// Used in the Audiopath Form.
    struct DMUS_IO_BUFFER_ATTRIBUTES_HEADER {
        /// Unique identifier of the buffer configuration.
        GUID guidBufferID;

        /// Flags describing the buffer.
        std::uint32_t dwFlags;
    };

    /// The DMUS_IO_CHORD structure contains information about a chord change.
    /// Used in the Chord Track List.
    struct DMUS_IO_CHORD {
        /// Name of the chord.
        std::uint16_t wszName[16];

        /// Time of the chord.
        std::uint32_t mtTime;

        /// Measure that the chord falls on.
        std::uint16_t wMeasure;

        /// Beat that the chord falls on.
        std::uint8_t bBeat;

        DMUS_CHORDKEYF_FLAGS bFlags;

        DMUS_IO_CHORD() {}
        DMUS_IO_CHORD(const std::uint8_t *data) {
            for (int i = 0; i < 16; i++) {
                wszName[i] = littleEndianRead<std::uint16_t>(data + 2 * i);
            }
            FIELDINIT(DMUS_IO_CHORD, mtTime, std::uint32_t);
            FIELDINIT(DMUS_IO_CHORD, wMeasure, std::uint16_t);
            FIELDINIT(DMUS_IO_CHORD, bBeat, std::uint8_t);
            FIELDINITC(DMUS_IO_CHORD, bFlags, std::uint8_t, DMUS_CHORDKEYF_FLAGS);
        }
    };

    /// The DMUS_IO_CHORDENTRY structure contains information about a chord entry.
    /// Used in the Chordmap Form.
    struct DMUS_IO_CHORDENTRY {
        /// Flag indicating whether the chord is a starting chord (bit 2 set)
        ///  or an ending chord (bit 3 set) in the chord graph.
        std::uint32_t dwFlags;

        /// Replaces the run-time pointer to this.
        /// Each chord entry is tagged with a unique connection identifier.
        std::uint16_t wConnectionID;

        DMUS_IO_CHORDENTRY() {}
        DMUS_IO_CHORDENTRY(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_CHORDENTRY, dwFlags, std::uint32_t);
            FIELDINIT(DMUS_IO_CHORDENTRY, wConnectionID, std::uint16_t);
        }
    };

    /// The DMUS_IO_CHORDMAP structure contains information about a chordmap.
    /// Used in the Chordmap Form.
    struct DMUS_IO_CHORDMAP {
        /// Name of the chordmap, used in the object description when the chordmap is loaded.
        std::uint16_t wszLoadName[20];

        /// Scale associated with the chordmap.
        /// Each of the lower 24 bits represents a semitone,
        ///  starting with the root at the least significant bit,
        ///  and the bit is set if the note is in the scale.
        std::uint32_t dwScalePattern;

        std::uint32_t dwFlags;

        DMUS_IO_CHORDMAP() {}
        DMUS_IO_CHORDMAP(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_CHORDMAP, dwScalePattern, std::uint32_t);
            FIELDINIT(DMUS_IO_CHORDMAP, dwFlags, std::uint32_t);
        }
    };

    /// The DMUS_IO_CHORDMAP_SIGNPOST structure contains information about a signpost chord in a chordmap.
    /// Used in the Chordmap Form.
    struct DMUS_IO_CHORDMAP_SIGNPOST {
        /// Types of signpost supported by this chord.
        /// The values are used to match against the same values as they appear in templates.
        /// Composing from template consists of (among other things) looking for these values
        ///  in the template and finding actual chords in the chordmap that match these values.
        std::uint32_t dwChords;

        /// Flags defining whether this chord is to be preceded by cadence chords.
        /// Signpost chords can have up to two cadence chords.
        /// This value can be SPOST_CADENCE1 (first cadence), SPOST_CADENCE2 (second cadence), or
        ///  a combination of these two flags.
        std::uint32_t dwFlags;

        DMUS_IO_CHORDMAP_SIGNPOST() {}
        DMUS_IO_CHORDMAP_SIGNPOST(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_CHORDMAP_SIGNPOST, dwChords, std::uint32_t);
            FIELDINIT(DMUS_IO_CHORDMAP_SIGNPOST, dwFlags, std::uint32_t);
        }
    };

    /// The DMUS_IO_CHORDMAP_SUBCHORD structure contains information about a subchord.
    /// Used in the Chordmap Form.
    struct DMUS_IO_CHORDMAP_SUBCHORD {
        /// Notes in the subchord.
        /// Each of the lower 24 bits represents a semitone,
        ///  starting with the root at the least significant bit,
        ///  and the bit is set if the note is in the chord.
        std::uint32_t dwChordPattern;

        /// Notes in the scale.
        /// Each of the lower 24 bits represents a semitone,
        ///  starting with the root at the least significant bit,
        ///  and the bit is set if the note is in the scale.
        std::uint32_t dwScalePattern;

        /// Points in the scale at which inversions can occur.
        /// Bits that are off signify that the notes in the interval cannot be inverted.
        /// Thus, the pattern 100001111111 indicates that inversions are allowed anywhere
        ///  except between the fifth and seventh degrees of a major scale.
        std::uint32_t dwInvertPattern;

        /// Root of the subchord, where 0 is the lowest C in the range and 23 is the top B.
        std::uint8_t bChordRoot;

        /// Root of the scale, where 0 is the lowest C in the range and 23 is the top B.
        std::uint8_t bScaleRoot;

        /// Reserved for future use.
        std::uint16_t wCFlags;

        /// Bit field showing which levels are supported by this subchord.
        /// Each part in a style is assigned a level, and this chord is used
        ///  only for parts whose levels are contained in this member.
        std::uint32_t dwLevels;

        DMUS_IO_CHORDMAP_SUBCHORD() {}
        DMUS_IO_CHORDMAP_SUBCHORD(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_CHORDMAP_SUBCHORD, dwChordPattern, std::uint32_t);
            FIELDINIT(DMUS_IO_CHORDMAP_SUBCHORD, dwScalePattern, std::uint32_t);
            FIELDINIT(DMUS_IO_CHORDMAP_SUBCHORD, dwInvertPattern, std::uint32_t);
            FIELDINIT(DMUS_IO_CHORDMAP_SUBCHORD, bChordRoot, std::uint8_t);
            FIELDINIT(DMUS_IO_CHORDMAP_SUBCHORD, bScaleRoot, std::uint8_t);
            FIELDINIT(DMUS_IO_CHORDMAP_SUBCHORD, wCFlags, std::uint16_t);
            FIELDINIT(DMUS_IO_CHORDMAP_SUBCHORD, dwLevels, std::uint32_t);
        }
    };


    /// The DMUS_IO_COMMAND structure contains information about a command event.
    /// Used in the Command Track Chunk.
    struct DMUS_IO_COMMAND {
        /// Time of the command.
        std::uint32_t mtTime;

        /// Measure that the command falls on.
        std::uint16_t wMeasure;

        /// Beat that the command falls on.
        std::uint8_t bBeat;

        /// Command type. See DMUS_COMMANDT_TYPES.
        DMUS_COMMANDT_TYPES bCommand;

        /// Groove level, or 0 if the command is not a groove command.
        std::uint8_t bGrooveLevel;

        /// Size of the range within which the groove level can be randomized.
        /// If this value is an odd number, the groove range is bGrooveRange � 1.
        /// For instance, if the groove level is 35 and bGrooveRange is 5,
        ///  the adjusted groove range is 4 and the groove level could be anywhere from 33 to 37.
        std::uint8_t bGrooveRange;

        /// Flag that specifies how patterns are selected for repetition. See DMUS_PATTERNT_TYPES.
        std::uint8_t bRepeatMode;

        DMUS_IO_COMMAND() {}
        DMUS_IO_COMMAND(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_COMMAND, mtTime, std::uint32_t);
            FIELDINIT(DMUS_IO_COMMAND, wMeasure, std::uint16_t);
            FIELDINIT(DMUS_IO_COMMAND, bBeat, std::uint8_t);
            bCommand = (DMUS_COMMANDT_TYPES)DirectMusic::littleEndianRead<std::uint8_t>(data + offsetof(DMUS_IO_COMMAND, bCommand));
            FIELDINIT(DMUS_IO_COMMAND, bGrooveLevel, std::uint8_t);
            FIELDINIT(DMUS_IO_COMMAND, bGrooveRange, std::uint8_t);
            FIELDINIT(DMUS_IO_COMMAND, bRepeatMode, std::uint8_t);
        }
    };

    /// The DMUS_IO_CONTAINED_OBJECT_HEADER structure is used before each object in a Container Form.
    struct DMUS_IO_CONTAINED_OBJECT_HEADER {
        /// Class identifier of the object.
        GUID guidClassID;

        std::uint32_t dwFlags;

        /// Identifier of the data chunk.
        /// If this value is zero, it is assumed that the chunk is of type LIST,
        ///  so fccType is valid and must be nonzero.
        char ckid[4];

        /// List type.
        /// If this value is zero, ckid is valid and must be nonzero.
        char fccType[4];
    };

    /// The DMUS_IO_CONTAINER_HEADER structure is used in the Container Form.
    struct DMUS_IO_CONTAINER_HEADER {
        std::uint32_t dwFlags;
    };

    /// The DMUS_IO_CURVE_ITEM structure contains information about a curve event in a track.
    /// Used in the Sequence Track Chunk.
    struct DMUS_IO_CURVE_ITEM {
        /// Start time of the curve.
        std::uint32_t mtStart;

        /// Duration of the curve.
        std::uint32_t mtDuration;

        /// Time after the curve is finished during which a reset can occur.
        std::uint32_t mtResetDuration;

        /// Performance channel for the event.
        std::uint32_t dwPChannel;

        /// Offset from the grid boundary at which the curve occurs, in music time.
        /// Because MIDI curves are associated with the closest grid when loaded,
        ///  this value can be positive or negative.
        std::uint16_t nOffset;

        /// Start value.
        std::uint16_t nStartValue;

        /// End value.
        std::uint16_t nEndValue;

        /// Type of curve.
        std::uint16_t nResetValue;

        /// Shape of curve. The following shapes are defined as shown.
        std::uint8_t bType;

        /// Shape of curve. The following shapes are defined as shown.
        std::uint8_t bCurveShape;

        /// CC number if this is a control change type.
        std::uint8_t bCCData;

        /// Set to DMUS_CURVE_RESET if the nResetValue must be set when an
        /// invalidation occurs because of a transition.
        /// If 0, the curve stays permanently at the new value. All other bits are reserved.
        std::uint8_t bFlags;

        /// Parameter number for RPN and NRPN types.
        std::uint16_t wParamType;

        /// Merge index. Supported for mod wheel, reverb send, chorus send, pitch bend, volume, and expression controllers.
        std::uint16_t wMergeIndex;

        DMUS_IO_CURVE_ITEM() {}
        DMUS_IO_CURVE_ITEM(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_CURVE_ITEM, mtStart, std::uint32_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, mtDuration, std::uint32_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, mtResetDuration, std::uint32_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, dwPChannel, std::uint32_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, nOffset, std::uint16_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, nStartValue, std::uint16_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, nEndValue, std::uint16_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, nResetValue, std::uint16_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, bType, std::uint8_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, bCurveShape, std::uint8_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, bCCData, std::uint8_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, bFlags, std::uint8_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, wParamType, std::uint16_t);
            FIELDINIT(DMUS_IO_CURVE_ITEM, wMergeIndex, std::uint16_t);
        }
    };

    /// The DMUS_IO_INSTRUMENT structure contains information about an instrument.
    /// Used in the Band Form.
    struct DMUS_IO_INSTRUMENT {
        /// MSB, LSB, and program change to define instrument.
        std::uint32_t dwPatch;

        /// MSB, LSB, and program change to assign to instrument when downloading.
        std::uint32_t dwAssignPatch;

        /// 128 bits; one for each MIDI note that the instrument must be able to play.
        std::uint32_t dwNoteRanges[4];

        /// Performance channel that the instrument plays on.
        std::uint32_t dwPChannel;

        /// Control flags.
        std::uint32_t dwFlags;

        /// Pan for the instrument.
        std::uint8_t bPan;

        /// Volume for the instrument.
        std::uint8_t bVolume;

        /// Number of semitones to transpose notes.
        std::uint16_t nTranspose;

        /// Channel priority.
        std::uint32_t dwChannelPriority;

        /// Number of semitones shifted by pitch bend.
        std::uint16_t nPitchBendRange;

        DMUS_IO_INSTRUMENT() {}
        DMUS_IO_INSTRUMENT(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_INSTRUMENT, dwPatch, std::uint32_t);
            FIELDINIT(DMUS_IO_INSTRUMENT, dwAssignPatch, std::uint32_t);
            dwNoteRanges[0] = littleEndianRead<std::uint32_t>(data + offsetof(DMUS_IO_INSTRUMENT, dwNoteRanges));
            dwNoteRanges[1] = littleEndianRead<std::uint32_t>(data + 4 + offsetof(DMUS_IO_INSTRUMENT, dwNoteRanges));
            dwNoteRanges[2] = littleEndianRead<std::uint32_t>(data + 8 + offsetof(DMUS_IO_INSTRUMENT, dwNoteRanges));
            dwNoteRanges[3] = littleEndianRead<std::uint32_t>(data + 12 + offsetof(DMUS_IO_INSTRUMENT, dwNoteRanges));
            FIELDINIT(DMUS_IO_INSTRUMENT, dwPChannel, std::uint32_t);
            FIELDINIT(DMUS_IO_INSTRUMENT, dwFlags, std::uint32_t);
            FIELDINIT(DMUS_IO_INSTRUMENT, bPan, std::uint8_t);
            FIELDINIT(DMUS_IO_INSTRUMENT, bVolume, std::uint8_t);
            FIELDINIT(DMUS_IO_INSTRUMENT, nTranspose, std::uint16_t);
            FIELDINIT(DMUS_IO_INSTRUMENT, dwChannelPriority, std::uint32_t);
            FIELDINIT(DMUS_IO_INSTRUMENT, nPitchBendRange, std::uint16_t);
        }
    };

    /// The DMUS_IO_LYRICSTRACK_EVENTHEADER structure is used in a Lyrics Track List.
    struct DMUS_IO_LYRICSTRACK_EVENTHEADER {
        /// Reserved; must be zero.
        std::uint32_t dwFlags;

        /// Flags to determine the timing of the notification.
        std::uint32_t dwTimingFlags;

        /// Time in the music with which the event is associated.
        std::uint32_t lTimeLogical;

        /// Precise time when the event will be triggered. This should be close to logical time.
        std::uint32_t lTimePhysical;

        DMUS_IO_LYRICSTRACK_EVENTHEADER() {}
        DMUS_IO_LYRICSTRACK_EVENTHEADER(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_LYRICSTRACK_EVENTHEADER, dwFlags, std::uint32_t);
            FIELDINIT(DMUS_IO_LYRICSTRACK_EVENTHEADER, dwTimingFlags, std::uint32_t);
            FIELDINIT(DMUS_IO_LYRICSTRACK_EVENTHEADER, lTimeLogical, std::uint32_t);
            FIELDINIT(DMUS_IO_LYRICSTRACK_EVENTHEADER, lTimePhysical, std::uint32_t);
        }
    };

    /// The DMUS_IO_MOTIFSETTINGS structure contains information about a motif.
    /// Used in the Style Form.
    struct DMUS_IO_MOTIFSETTINGS {
        /// Number of repetitions.
        std::uint32_t dwRepeats;

        /// Start of playback, normally 0.
        std::uint32_t mtPlayStart;

        /// Start of looping portion, normally 0.
        std::uint32_t mtLoopStart;

        /// End of looping portion. Must be greater than mtLoopStart, or zero to loop the entire motif.
        std::uint32_t mtLoopEnd;

        /// Default resolution. See DMUS_TIME_RESOLVE_FLAGS.
        std::uint32_t dwResolution;

        DMUS_IO_MOTIFSETTINGS() {}
        DMUS_IO_MOTIFSETTINGS(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_MOTIFSETTINGS, dwRepeats, std::uint32_t);
            FIELDINIT(DMUS_IO_MOTIFSETTINGS, mtPlayStart, std::uint32_t);
            FIELDINIT(DMUS_IO_MOTIFSETTINGS, mtLoopStart, std::uint32_t);
            FIELDINIT(DMUS_IO_MOTIFSETTINGS, mtLoopEnd, std::uint32_t);
            FIELDINIT(DMUS_IO_MOTIFSETTINGS, dwResolution, std::uint32_t);
        }
    };

    /// The DMUS_IO_MUTE structure contains information about a mute event on a channel.
    /// Used in the Mute Track Chunk.
    struct DMUS_IO_MUTE {
        /// Time of the event.
        std::uint32_t mtTime;

        /// Performance channel to mute or remap.
        std::uint32_t dwPChannel;

        /// Channel to which dwPChannel is being mapped, or 0xFFFFFFFF if dwPChannel is to be muted.
        std::uint32_t dwPChannelMap;

        DMUS_IO_MUTE() {}
        DMUS_IO_MUTE(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_MUTE, mtTime, std::uint32_t);
            FIELDINIT(DMUS_IO_MUTE, dwPChannel, std::uint32_t);
            FIELDINIT(DMUS_IO_MUTE, dwPChannelMap, std::uint32_t);
        }
    };

    /// The DMUS_IO_NEXTCHORD structure contains information about the next chord in a chord graph.
    /// Used in the Chordmap Form.
    struct DMUS_IO_NEXTCHORD {
        /// Reserved for future use.
        std::uint32_t dwFlags;

        /// Likelihood (in the range from 1 through 100) that this link is followed when traversing the chord graph.
        std::uint16_t nWeight;

        /// Smallest number of beats that this chord is allowed to play in a composed segment.
        std::uint16_t wMinBeats;

        /// Largest number of beats that this chord is allowed to play in a composed segment.
        std::uint16_t wMaxBeats;

        /// Refers to the wConnectionID member of a DMUS_IO_CHORDENTRY structure.
        std::uint16_t wConnectionID;

        DMUS_IO_NEXTCHORD() {}
        DMUS_IO_NEXTCHORD(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_NEXTCHORD, dwFlags, std::uint32_t);
            FIELDINIT(DMUS_IO_NEXTCHORD, nWeight, std::uint16_t);
            FIELDINIT(DMUS_IO_NEXTCHORD, wMinBeats, std::uint16_t);
            FIELDINIT(DMUS_IO_NEXTCHORD, wMaxBeats, std::uint16_t);
            FIELDINIT(DMUS_IO_NEXTCHORD, wConnectionID, std::uint16_t);
        }
    };

    /// The DMUS_IO_PARAMCONTROLTRACK_CURVEINFO structure is used in a Parameter Control Track List.
    struct DMUS_IO_PARAMCONTROLTRACK_CURVEINFO {
        /// Start time of the curve.
        std::uint32_t mtStartTime;

        /// End time of the curve.
        std::uint32_t mtEndTime;

        /// Start value of the curve.
        float fltStartValue;

        /// End value of the curve.
        float fltEndValue;

        /// Item from the MP_CURVE_TYPE enumeration.
        std::uint32_t dwCurveType;

        /// Combination of the MPF_ENVLP_* constants.
        std::uint32_t dwFlags;
    };

    /// The DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER structure is used in a Parameter Control Track List.
    struct DMUS_IO_PARAMCONTROLTRACK_OBJECTHEADER {
        /// Reserved; must be zero.
        std::uint32_t dwFlags;

        /// Time format to set the object to. Must be GUID_TIME_REFERENCE or GUID_TIME_MUSIC.
        GUID guidTimeFormat;

        /// Performance channel, 0, or DMUS_PCHANNEL_ALL.
        std::uint32_t dwPChannel;

        /// Stage in the path.
        std::uint32_t dwStage;

        /// Index of the buffer, if there is more than one.
        std::uint32_t dwBuffer;

        /// Class identifier of the object, such as GUID_DSFX_STANDARD_CHORUS.
        GUID guidObject;

        /// Index of the object in the list of matching objects.
        std::uint32_t dwIndex;
    };

    /// The DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER structure is used in a Parameter Control Track List.
    struct DMUS_IO_PARAMCONTROLTRACK_PARAMHEADER {
        /// Reserved; must be zero.
        std::uint32_t dwFlags;

        /// Index number of the parameter on the object.
        std::uint32_t dwIndex;
    };

    /// The DMUS_IO_PARTREF structure contains information about a part reference.
    /// Used in the Style Form.
    struct DMUS_IO_PARTREF {
        /// Identifier of the part.
        GUID  guidPartID;

        /// Identifier corresponding to a particular MIDI channel on a port.
        /// This member has been superseded by dwPChannel and is no longer used.
        std::uint16_t wLogicalPartID;

        /// Variation lock identifier.
        /// Parts with the same value in this member always play the same variation.
        /// A value of 0 means that the part plays its variations independently of all other parts.
        std::uint8_t bVariationLockID;

        /// Subchord level that this part wants.
        std::uint8_t bSubChordLevel;

        /// Reserved for future use.
        std::uint8_t bPriority;

        /// Can be 0, meaning that matching variations play sequentially, or one
        /// of the members of the DMUS_VARIATIONT_TYPES enumeration.
        std::uint8_t bRandomVariation;

        /// Padding for alignment; value not used.
        std::uint16_t wPad;

        /// Performance channel of the part.
        std::uint32_t dwPChannel;

        DMUS_IO_PARTREF() {}
        DMUS_IO_PARTREF(const std::uint8_t *data)
            : FIELDCONST(DMUS_IO_PARTREF, guidPartID, GUID)
        {
            FIELDINIT(DMUS_IO_PARTREF, wLogicalPartID, std::uint16_t);
            FIELDINIT(DMUS_IO_PARTREF, bVariationLockID, std::uint8_t);
            FIELDINIT(DMUS_IO_PARTREF, bSubChordLevel, std::uint8_t);
            FIELDINIT(DMUS_IO_PARTREF, bPriority, std::uint8_t);
            FIELDINIT(DMUS_IO_PARTREF, bRandomVariation, std::uint8_t);
            FIELDINIT(DMUS_IO_PARTREF, wPad, std::uint16_t);
            FIELDINIT(DMUS_IO_PARTREF, dwPChannel, std::uint32_t);
        }
    };

    /// The DMUS_IO_PATTERN structure contains information about a pattern. Used in the Style Form.
    struct DMUS_IO_PATTERN {
        /// DMUS_IO_TIMESIG structure containing a time signature to override the style's default time signature.
        DMUS_IO_TIMESIG timeSig;

        /// Bottom of the groove range.
        std::uint8_t bGrooveBottom;

        /// Top of the groove range.
        std::uint8_t bGrooveTop;

        /// Type of embellishment.
        /// One or more of the constants from the DMUS_EMBELLISHT_TYPES enumeration,
        /// or a value defined by the content provider,
        /// such as a custom embellishment number assigned in DirectMusic Producer.
        std::uint16_t wEmbellishment;

        /// Length of the pattern in measures.
        std::uint16_t wNbrMeasures;

        /// Bottom of groove range for next pattern.
        std::uint8_t bDestGrooveBottom;

        /// Top of groove range for next pattern.
        std::uint8_t bDestGrooveTop;
        std::uint32_t dwFlags;

        DMUS_IO_PATTERN() {}
        DMUS_IO_PATTERN(const std::uint8_t *data)
            : FIELDCONST(DMUS_IO_PATTERN, timeSig, DMUS_IO_TIMESIG)
        {
            FIELDINIT(DMUS_IO_PATTERN, bGrooveBottom, std::uint8_t);
            FIELDINIT(DMUS_IO_PATTERN, bGrooveTop, std::uint8_t);
            FIELDINIT(DMUS_IO_PATTERN, wEmbellishment, std::uint16_t);
            FIELDINIT(DMUS_IO_PATTERN, wNbrMeasures, std::uint16_t);
            FIELDINIT(DMUS_IO_PATTERN, bDestGrooveBottom, std::uint8_t);
            FIELDINIT(DMUS_IO_PATTERN, bDestGrooveTop, std::uint8_t);
            FIELDINIT(DMUS_IO_PATTERN, dwFlags, std::uint32_t);
        }
    };

    /// The DMUS_IO_PCHANNELTOBUFFER_HEADER structure defines a range of
    /// performance channels and the buffers they connect to.
    /// Used in the port configuration list of an Audiopath Form.
    struct DMUS_IO_PCHANNELTOBUFFER_HEADER {
        /// First performance channel.
        std::uint32_t dwPChannelBase;

        /// Number of performance channels.
        std::uint32_t dwPChannelCount;

        /// Number of buffers the channels connect to.
        std::uint32_t dwBufferCount;

        /// Reserved. Must be 0.
        std::uint32_t dwFlags;
    };

    /// The DMUS_IO_PLAYMARKER structure contains information about a play marker,
    /// which is a point within a primary segment where a new segment is permitted to start playing.
    /// Used in the Marker Track List.
    struct DMUS_IO_PLAY_MARKER {
        /// Time of legal play point.
        std::uint32_t mtTime;

        DMUS_IO_PLAY_MARKER() {}
        DMUS_IO_PLAY_MARKER(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_PLAY_MARKER, mtTime, std::uint32_t);
        }
    };

    /// The DMUS_IO_PORTCONFIG_HEADER structure contains information about a port configuration.
    /// It is used in the Audiopath Form.
    struct DMUS_IO_PORTCONFIG_HEADER {
        /// Unique identifier of port.
        GUID  guidPort;

        /// First performance channel.
        std::uint32_t dwPChannelBase;

        /// Number of performance channels.
        std::uint32_t dwPChannelCount;
        std::uint32_t dwFlags;
    };

    /// The DMUS_IO_REFERENCE structure contains information about a reference
    /// to another object that might be stored in another file.
    /// Used in the Reference List chunk.
    struct DMUS_IO_REFERENCE {
        /// Class identifier.
        GUID guidClassID;

        /// Flags to indicate which data chunks for the reference are present.
        std::uint32_t dwValidData;

        DMUS_IO_REFERENCE() {}
        DMUS_IO_REFERENCE(const std::uint8_t *data)
            : FIELDCONST(DMUS_IO_REFERENCE, guidClassID, GUID)
        {
            FIELDINIT(DMUS_IO_REFERENCE, dwValidData, std::uint32_t);
        }
    };

    /// The DMUS_IO_SCRIPT_HEADER structure is used in the Script Form.
    struct DMUS_IO_SCRIPT_HEADER {
        /// DWORD value that specifies the loading behavior of the script.
        std::uint32_t dwFlags;
    };

    /// The DMUS_IO_SCRIPTTRACK_EVENTHEADER structure is used in a Script Track List.
    struct DMUS_IO_SCRIPTTRACK_EVENTHEADER {
        /// Flag that determines when the event is set.
        std::uint32_t dwFlags;

        /// Logical time of the event.
        std::uint32_t lTimeLogical;

        /// Actual time of the event.
        std::uint32_t lTimePhysical;
    };

    /// The DMUS_IO_SEGMENT_HEADER structure contains information about a segment.
    /// Used in the Segment Form.
    struct DMUS_IO_SEGMENT_HEADER {
        /// Number of repetitions.
        std::uint32_t dwRepeats;

        /// Length of the segment.
        std::uint32_t mtLength;

        /// Start of playback, normally 0.
        std::uint32_t mtPlayStart;

        /// Start of the looping portion, normally 0.
        std::uint32_t mtLoopStart;

        /// End of the looping portion.
        /// Must be greater than mtPlayStart, or zero to loop the entire segment.
        std::uint32_t mtLoopEnd;

        /// Default resolution.
        /// See DMUS_TIME_RESOLVE_FLAGS.
        std::uint32_t dwResolution;

        /// Length of the segment in reference time.
        /// Valid if the DMUS_SEGIOF_REFLENGTH flag is set.
        std::uint64_t rtLength;
        std::uint32_t dwFlags;

        /// Reserved.
        std::uint32_t dwReserved;

        /// Loop start in clock time.
        std::uint64_t rtLoopStart;

        /// Loop end in clock time.
        std::uint64_t rtLoopEnd;

        /// Start point in clock time.
        std::uint64_t rtPlayStart;

        bool operator==(const DMUS_IO_SEGMENT_HEADER& a) const {
            return a.dwRepeats == dwRepeats &&
                a.mtLength == mtLength &&
                a.mtPlayStart == mtPlayStart &&
                a.mtLoopStart == mtLoopStart &&
                a.mtLoopEnd == mtLoopEnd &&
                a.dwResolution == dwResolution &&
                a.rtLength == rtLength &&
                a.dwFlags == dwFlags &&
                a.dwReserved == dwReserved &&
                a.rtLoopStart == rtLoopStart &&
                a.rtLoopEnd == rtLoopEnd;
        }

        DMUS_IO_SEGMENT_HEADER() {}
        DMUS_IO_SEGMENT_HEADER(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_SEGMENT_HEADER, dwRepeats, std::uint32_t);
            FIELDINIT(DMUS_IO_SEGMENT_HEADER, mtLength, std::uint32_t);
            FIELDINIT(DMUS_IO_SEGMENT_HEADER, mtPlayStart, std::uint32_t);
            FIELDINIT(DMUS_IO_SEGMENT_HEADER, mtLoopStart, std::uint32_t);
            FIELDINIT(DMUS_IO_SEGMENT_HEADER, mtLoopEnd, std::uint32_t);
            FIELDINIT(DMUS_IO_SEGMENT_HEADER, dwResolution, std::uint32_t);
            FIELDINIT(DMUS_IO_SEGMENT_HEADER, rtLength, std::uint64_t);
            FIELDINIT(DMUS_IO_SEGMENT_HEADER, dwFlags, std::uint32_t);
            FIELDINIT(DMUS_IO_SEGMENT_HEADER, dwReserved, std::uint32_t);
            FIELDINIT(DMUS_IO_SEGMENT_HEADER, rtLoopStart, std::uint64_t);
            FIELDINIT(DMUS_IO_SEGMENT_HEADER, rtLoopEnd, std::uint32_t);
        }
    };

    /// The DMUS_IO_SEGMENT_ITEM_HEADER structure contains information about
    /// a segment referenced in the Segment Trigger Track List.
    struct DMUS_IO_SEGMENT_ITEM_HEADER {
        /// Time in the music with which the event is associated.
        std::uint32_t lTimeLogical;

        /// Actual time at which the segment is to play.
        std::uint32_t lTimePhysical;

        /// See DMUS_SEGF_FLAGS.
        std::uint32_t dwPlayFlags;
        std::uint32_t dwFlags;
    };

    /// The DMUS_IO_SEGMENT_TRACK_HEADER structure contains information about a Segment Trigger Track List.
    struct DMUS_IO_SEGMENT_TRACK_HEADER {
        /// Reserved. Must be zero.
        std::uint32_t dwFlags;
    };

    /// The DMUS_IO_SEQ_ITEM structure contains information about an item of data in a sequence track.
    /// Used in the Sequence Track Chunk.
    struct DMUS_IO_SEQ_ITEM {
        /// Logical time of the event.
        std::uint32_t mtTime;

        /// Duration for which the event is valid.
        std::uint32_t mtDuration;

        /// Performance channel for the event.
        std::uint32_t dwPChannel;

        /// Offset from mtTime at which the note is played, in music time.
        std::int16_t nOffset;

        /// MIDI event type. Equivalent to the MIDI status byte, but without channel information.
        std::uint8_t bStatus;

        /// First byte of the MIDI data.
        std::uint8_t bByte1;

        /// Second byte of the MIDI data.
        std::uint8_t bByte2;

        DMUS_IO_SEQ_ITEM() {}
        DMUS_IO_SEQ_ITEM(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_SEQ_ITEM, mtTime, std::uint32_t);
            FIELDINIT(DMUS_IO_SEQ_ITEM, mtDuration, std::uint32_t);
            FIELDINIT(DMUS_IO_SEQ_ITEM, dwPChannel, std::uint32_t);
            FIELDINIT(DMUS_IO_SEQ_ITEM, nOffset, std::int16_t);
            FIELDINIT(DMUS_IO_SEQ_ITEM, bStatus, std::uint8_t);
            FIELDINIT(DMUS_IO_SEQ_ITEM, bByte1, std::uint8_t);
            FIELDINIT(DMUS_IO_SEQ_ITEM, bByte2, std::uint8_t);
        }
    };

    /// The DMUS_IO_SIGNPOST structure contains information about a signpost in a signpost track
    /// to associate it with signpost chords in a chordmap.
    /// Used in the Signpost Track Chunk.
    struct DMUS_IO_SIGNPOST {
        /// Time of the signpost.
        std::uint32_t mtTime;

        /// Types of signpost chords allowed to be associated with this signpost.
        /// The values are used to match against the same values as they appear in templates.
        /// Composing from a template consists of (among other things) looking for these values
        /// in the template and finding actual chords in the chordmap that match these values.
        std::uint32_t dwChords;

        /// Measure on which the signpost falls.
        std::uint16_t wMeasure;

        DMUS_IO_SIGNPOST() {}
        DMUS_IO_SIGNPOST(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_SIGNPOST, mtTime, std::uint32_t);
            FIELDINIT(DMUS_IO_SIGNPOST, dwChords, std::uint32_t);
            FIELDINIT(DMUS_IO_SIGNPOST, wMeasure, std::uint16_t);
        }
    };

    /// The DMUS_IO_STYLE structure contains information about the time signature and tempo of a style.
    /// Used in the Style Form.
    struct DMUS_IO_STYLE {
        /// DMUS_IO_TIMESIG structure containing the default time signature for the style.
        DMUS_IO_TIMESIG timeSig;

        /// Tempo of the style.
        double dblTempo;

        bool operator==(const DMUS_IO_STYLE& a) const {
            return a.timeSig == timeSig &&
                a.dblTempo == dblTempo;
        }

        DMUS_IO_STYLE() {}
        DMUS_IO_STYLE(const std::uint8_t *data)
            : timeSig(data)
        {
            dblTempo = *((double*)(data + offsetof(DMUS_IO_STYLE, dblTempo)));
        }
    };

    /// The DMUS_IO_STYLE_ANTICIPATION structure describes a resolution anticipation.
    /// Used in the Style Form.
    struct DMUS_IO_STYLE_ANTICIPATION {
        /// Offset within the part, in grids, at which the event is to play.
        std::uint32_t mtGridStart;

        /// Variations, where each bit set specifies a valid variation.
        std::uint32_t dwVariation;

        /// Offset of the time from mtGridStart.
        std::uint16_t nTimeOffset;

        /// Range by which to randomize time.
        std::uint8_t bTimeRange;

        DMUS_IO_STYLE_ANTICIPATION() {}
        DMUS_IO_STYLE_ANTICIPATION(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_STYLE_ANTICIPATION, mtGridStart, std::uint32_t);
            FIELDINIT(DMUS_IO_STYLE_ANTICIPATION, dwVariation, std::uint32_t);
            FIELDINIT(DMUS_IO_STYLE_ANTICIPATION, nTimeOffset, std::uint16_t);
            FIELDINIT(DMUS_IO_STYLE_ANTICIPATION, bTimeRange, std::uint8_t);
        }
    };

    /// The DMUS_IO_STYLECURVE structure contains information about a curve in a style.
    /// Used in the Style Form.
    struct DMUS_IO_STYLECURVE {
        /// Offset, in grids, at which the curve occurs.
        std::uint32_t mtGridStart;

        /// Variations that this curve belongs to.
        /// Each bit corresponds to one of 32 variations.
        std::uint32_t dwVariation;

        /// Duration of the curve.
        std::uint32_t mtDuration;

        /// Time after the curve is finished during which a reset can occur.
        std::uint32_t mtResetDuration;

        /// Offset from mtGridStart at which the curve occurs.
        std::uint16_t nTimeOffset;

        /// Start value.
        std::uint16_t nStartValue;

        /// End value.
        std::uint16_t nEndValue;

        /// Reset value, set upon a flush or invalidation during the time specified by mtResetDuration.
        std::uint16_t nResetValue;

        /// Type of curve. See DMUS_IO_CURVE_ITEM.
        std::uint8_t bEventType;

        /// Shape of curve. See DMUS_IO_CURVE_ITEM.
        std::uint8_t bCurveShape;

        /// CC number if this is a control change type.
        std::uint8_t bCCData;

        /// Set to DMUS_CURVE_RESET if the nResetValue must be set when an invalidation
        /// occurs because of a transition.
        /// If 0, the curve stays permanently at the new value.
        /// All other bits are reserved.
        std::uint8_t bFlags;

        /// RPN or NRPN parameter number.
        std::uint16_t wParamType;

        /// Merge index. Supported for mod wheel, reverb send, chorus send,
        /// pitch bend, volume, and expression controllers.
        std::uint16_t wMergeIndex;

        DMUS_IO_STYLECURVE() {}
        DMUS_IO_STYLECURVE(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_STYLECURVE, mtGridStart, std::uint32_t);
            FIELDINIT(DMUS_IO_STYLECURVE, dwVariation, std::uint32_t);
            FIELDINIT(DMUS_IO_STYLECURVE, mtDuration, std::uint32_t);
            FIELDINIT(DMUS_IO_STYLECURVE, mtResetDuration, std::uint32_t);
            FIELDINIT(DMUS_IO_STYLECURVE, nTimeOffset, std::uint16_t);
            FIELDINIT(DMUS_IO_STYLECURVE, nStartValue, std::uint16_t);
            FIELDINIT(DMUS_IO_STYLECURVE, nEndValue, std::uint16_t);
            FIELDINIT(DMUS_IO_STYLECURVE, nResetValue, std::uint16_t);
            FIELDINIT(DMUS_IO_STYLECURVE, bEventType, std::uint8_t);
            FIELDINIT(DMUS_IO_STYLECURVE, bCurveShape, std::uint8_t);
            FIELDINIT(DMUS_IO_STYLECURVE, bCCData, std::uint8_t);
            FIELDINIT(DMUS_IO_STYLECURVE, bFlags, std::uint8_t);
            FIELDINIT(DMUS_IO_STYLECURVE, wParamType, std::uint16_t);
            FIELDINIT(DMUS_IO_STYLECURVE, wMergeIndex, std::uint16_t);
        }
    };

    /// The DMUS_IO_STYLEMARKER structure contains information about a marker in a style.
    /// Used in the Style Form.
    struct DMUS_IO_STYLEMARKER {
        /// Offset, in grids, at which the marker occurs.
        std::uint32_t mtGridStart;

        /// Variations that this marker belongs to.
        /// Each bit corresponds to one of 32 variations.
        std::uint32_t dwVariation;

        /// Flags that specify behavior of the marker.
        /// If zero, the behavior is as it was in DirectX version 7.0.
        std::uint16_t wMarkerFlags;
        
        DMUS_IO_STYLEMARKER() {}
        DMUS_IO_STYLEMARKER(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_STYLEMARKER, mtGridStart, std::uint32_t);
            FIELDINIT(DMUS_IO_STYLEMARKER, dwVariation, std::uint32_t);
            FIELDINIT(DMUS_IO_STYLEMARKER, wMarkerFlags, std::uint16_t);
        }
    };

    /// The DMUS_IO_STYLENOTE structure contains information about a note in a style.
    /// Used in the Style Form.
    struct DMUS_IO_STYLENOTE {
        /// Offset, in grids, at which the note occurs.
        std::uint32_t mtGridStart;

        /// Variations that this note belongs to.
        /// Each bit corresponds to one of 32 variations.
        std::uint32_t dwVariation;

        /// Duration of the note.
        std::uint32_t mtDuration;

        /// Time after mtGridStart at which the event occurs.
        std::int16_t nTimeOffset;

        /// Position in the scale.
        std::uint16_t wMusicValue;

        /// Note velocity.
        std::uint8_t bVelocity;

        /// Range within which to randomize start time.
        std::uint8_t bTimeRange;

        /// Range within which to randomize duration.
        std::uint8_t bDurRange;

        /// Range within which to randomize velocity.
        std::uint8_t bVelRange;

        /// Identifier of inversion group to which this note belongs.
        std::uint8_t bInversionID;

        /// Flags to override the play mode of the part.
        /// For a list of values, see DMUS_PLAYMODE_FLAGS.
        DMUS_PLAYMODE_FLAGS bPlayModeFlags;

        /// Flags. See DMUS_NOTEF_FLAGS.
        DMUS_NOTEF_FLAGS bNoteFlags;

        DMUS_IO_STYLENOTE() {}
        DMUS_IO_STYLENOTE(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_STYLENOTE, mtGridStart, std::uint32_t);
            FIELDINIT(DMUS_IO_STYLENOTE, dwVariation, std::uint32_t);
            FIELDINIT(DMUS_IO_STYLENOTE, mtDuration, std::uint32_t);
            FIELDINIT(DMUS_IO_STYLENOTE, nTimeOffset, std::int16_t);
            FIELDINIT(DMUS_IO_STYLENOTE, wMusicValue, std::uint16_t);
            FIELDINIT(DMUS_IO_STYLENOTE, bVelocity, std::uint8_t);
            FIELDINIT(DMUS_IO_STYLENOTE, bTimeRange, std::uint8_t);
            FIELDINIT(DMUS_IO_STYLENOTE, bDurRange, std::uint8_t);
            FIELDINIT(DMUS_IO_STYLENOTE, bVelRange, std::uint8_t);
            FIELDINIT(DMUS_IO_STYLENOTE, bInversionID, std::uint8_t);
            FIELDINITC(DMUS_IO_STYLENOTE, bPlayModeFlags, std::uint8_t, DMUS_PLAYMODE_FLAGS);
            FIELDINITC(DMUS_IO_STYLENOTE, bNoteFlags, std::uint8_t, DMUS_NOTEF_FLAGS);
        }
    };

    /// The DMUS_IO_STYLEPART structure contains information about a musical part.
    /// Used in the Style Form.
    struct DMUS_IO_STYLEPART {
        /// DMUS_IO_TIMESIG structure containing a time signature to override the style's default time signature.
        DMUS_IO_TIMESIG timeSig;

        /// Each element corresponds to one of 32 possible variations.
        /// The flags set in each element indicate which types of chord are supported by that variation.
        /// One of the mode masks is also set to indicate whether the variations are in DirectMusic or IMA mode.
        std::array<std::uint32_t, 32> dwVariationChoices;

        /// Unique identifier of the part.
        GUID guidPartID;

        /// Length of the part, in measures.
        std::uint16_t wNbrMeasures;

        /// Flags to define the play mode. For a list of values, see DMUS_PLAYMODE_FLAGS.
        DMUS_PLAYMODE_FLAGS bPlayModeFlags;

        /// Upper limit of inversion.
        std::uint8_t bInvertUpper;

        /// Lower limit of inversion.
        std::uint8_t bInvertLower;

        /// Unused.
        std::uint8_t bPad[3];

        /// Flags that specify the behavior of the part.
        std::uint16_t dwFlags;

        DMUS_IO_STYLEPART() {}
        DMUS_IO_STYLEPART(const std::uint8_t *data)
            : FIELDCONST(DMUS_IO_STYLEPART, timeSig, DMUS_IO_TIMESIG),
              FIELDCONST(DMUS_IO_STYLEPART, guidPartID, GUID)
        {
            for (int i = 0; i < 32; i++) {
                dwVariationChoices[i] = littleEndianRead<std::uint32_t>(data + offsetof(DMUS_IO_STYLEPART, dwVariationChoices) + i * 4);
            }
            FIELDINIT(DMUS_IO_STYLEPART, wNbrMeasures, std::uint16_t);
            FIELDINITC(DMUS_IO_STYLEPART, bPlayModeFlags, std::uint8_t, DMUS_PLAYMODE_FLAGS);
            FIELDINIT(DMUS_IO_STYLEPART, bInvertUpper, std::uint8_t);
            FIELDINIT(DMUS_IO_STYLEPART, bInvertLower, std::uint8_t);
            for (int i = 0; i < 3; i++) {
                bPad[i] = littleEndianRead<std::uint32_t>(data + offsetof(DMUS_IO_STYLEPART, bPad) + i);
            }
            FIELDINIT(DMUS_IO_STYLEPART, dwFlags, std::uint16_t);
        }
    };

    /// The DMUS_IO_STYLERESOLUTION structure describes a style resolution.
    /// Used in the Style Form.
    struct DMUS_IO_STYLERESOLUTION {
        /// Variations, where each bit specifies a valid variation.
        std::uint32_t dwVariation;

        /// Position in scale.
        std::uint16_t wMusicValue;

        /// Inversion group to which this note belongs.
        std::uint8_t bInversionID;

        /// Play mode flags. See DMUS_PLAYMODE_FLAGS.
        DMUS_PLAYMODE_FLAGS bPlayModeFlags;

        DMUS_IO_STYLERESOLUTION() {}
        DMUS_IO_STYLERESOLUTION(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_STYLERESOLUTION, dwVariation, std::uint32_t);
            FIELDINIT(DMUS_IO_STYLERESOLUTION, wMusicValue, std::uint16_t);
            FIELDINIT(DMUS_IO_STYLERESOLUTION, bInversionID, std::uint8_t);
            FIELDINITC(DMUS_IO_STYLERESOLUTION, bPlayModeFlags, std::uint8_t, DMUS_PLAYMODE_FLAGS);
        }
    };

    /// The DMUS_IO_SUBCHORD structure contains information about a subchord.
    /// Used in the Chord Track List.
    struct DMUS_IO_SUBCHORD {
        /// Notes in the subchord.
        /// Each of the lower 24 bits represents a semitone, starting with the
        /// root at the least significant bit, and the bit is set if the note is in the chord.
        std::uint32_t dwChordPattern;

        /// Notes in the scale. Each of the lower 24 bits represents a semitone,
        /// starting with the root at the least significant bit, and the bit is
        /// set if the note is in the scale.
        std::uint32_t dwScalePattern;

        /// Points in the scale at which inversions can occur.
        /// Bits that are off signify that the notes in the interval cannot be inverted.
        /// Thus, the pattern 100001111111 indicates that inversions are allowed
        /// anywhere except between the fifth and seventh degrees of a major scale.
        std::uint32_t dwInversionPoints;

        /// Which levels are supported by this subchord.
        /// Certain instruments can be assigned different levels
        /// (such as to play only the lower subchords of a chord),
        /// and this value is a way of mapping subchords to those levels.
        std::uint32_t dwLevels;

        /// Root of the subchord, where 0 is the lowest C in the range and 23 is the top B.
        std::uint8_t bChordRoot;

        /// Root of the scale, where 0 is the lowest C in the range and 23 is the top B.
        std::uint8_t bScaleRoot;

        DMUS_IO_SUBCHORD() {}
        DMUS_IO_SUBCHORD(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_SUBCHORD, dwChordPattern, std::uint32_t);
            FIELDINIT(DMUS_IO_SUBCHORD, dwScalePattern, std::uint32_t);
            FIELDINIT(DMUS_IO_SUBCHORD, dwInversionPoints, std::uint32_t);
            FIELDINIT(DMUS_IO_SUBCHORD, dwLevels, std::uint32_t);
            FIELDINIT(DMUS_IO_SUBCHORD, bChordRoot, std::uint8_t);
            FIELDINIT(DMUS_IO_SUBCHORD, bScaleRoot, std::uint8_t);
        }
    };

    /// The DMUS_IO_SYSEX_ITEM structure contains information about a system exclusive MIDI message.
    /// Used in the Sysex Track Chunk.
    struct DMUS_IO_SYSEX_ITEM {
        /// Time of the message.
        std::uint32_t mtTime;

        /// Performance channel of the event.
        std::uint32_t dwPChannel;

        /// Length of the data, in bytes.
        std::uint32_t dwSysExLength;
    };

    /// The DMUS_IO_TIMESIGNATURE_ITEM structure contains information about a time signature change.
    /// Used in the Time Signature Track List.
    struct DMUS_IO_TIMESIGNATURE_ITEM {
        /// Time of the event.
        std::uint32_t lTime;

        /// Beats per measure (top of time signature).
        std::uint8_t bBeatsPerMeasure;

        /// Note that receives the beat (bottom of the time signature), where 1 is a whole note,
        /// 2 is a half note, 4 is a quarter note, and so on. Zero is a 256th note.
        std::uint8_t bBeat;

        /// Grids (subdivisions) per beat.
        std::uint16_t wGridsPerBeat;

        DMUS_IO_TIMESIGNATURE_ITEM() {}
        DMUS_IO_TIMESIGNATURE_ITEM(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_TIMESIGNATURE_ITEM, lTime, std::uint32_t);
            FIELDINIT(DMUS_IO_TIMESIGNATURE_ITEM, bBeatsPerMeasure, std::uint8_t);
            FIELDINIT(DMUS_IO_TIMESIGNATURE_ITEM, bBeat, std::uint8_t);
            FIELDINIT(DMUS_IO_TIMESIGNATURE_ITEM, wGridsPerBeat, std::uint16_t);
        }
    };

    /// The DMUS_IO_TOOL_HEADER structure contains information about a tool.
    /// Used in the Tool Form.
    struct DMUS_IO_TOOL_HEADER {
        /// Class identifier of the tool.
        GUID guidClassID;

        /// Position in the graph.
        std::int32_t lIndex;

        /// Number of items in the dwPChannels array.
        std::uint32_t cPChannels;

        /// Identifier of tool's data chunk.
        /// If this value is 0, it is assumed that the chunk is of type LIST,
        /// so fccType is valid and must be nonzero.
        char ckid[4];

        /// List type. If this value is 0, ckid is valid and must be nonzero.
        char fccType[4];

        /// Array of performance channels for which the tool is valid.
        std::uint32_t dwPChannels[1];
    };

    /// The DMUS_IO_TRACK_EXTRAS_HEADER structure is used in the Track Form.
    struct DMUS_IO_TRACK_EXTRAS_HEADER {
        /// Flags for control tracks.
        std::uint32_t dwFlags;

        /// Priority for composition.
        std::uint32_t dwPriority;

        DMUS_IO_TRACK_EXTRAS_HEADER() {}
        DMUS_IO_TRACK_EXTRAS_HEADER(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_TRACK_EXTRAS_HEADER, dwFlags, std::uint32_t);
            FIELDINIT(DMUS_IO_TRACK_EXTRAS_HEADER, dwPriority, std::uint32_t);
        }
    };

    /// The DMUS_IO_TRACK_HEADER structure contains information about a track.
    /// Used in the Track Form.
    struct DMUS_IO_TRACK_HEADER {
        /// Class identifier of the track.
        GUID guidClassID;

        /// Position in the track list.
        std::uint32_t dwPosition;

        /// Group bits for the track.
        std::uint32_t dwGroup;

        /// Identifier of the track's data chunk.
        /// If this value is 0, it is assumed that the chunk is of type LIST,
        /// so fccType is valid and must be nonzero.
        char ckid[4];

        /// List type. If this value is 0, ckid is valid and must be nonzero.
        char fccType[4];

        DMUS_IO_TRACK_HEADER() {}
        DMUS_IO_TRACK_HEADER(const std::uint8_t *data)
            : FIELDCONST(DMUS_IO_TRACK_HEADER, guidClassID, GUID)
        {
            FIELDINIT(DMUS_IO_TRACK_HEADER, dwPosition, std::uint32_t);
            FIELDINIT(DMUS_IO_TRACK_HEADER, dwGroup, std::uint32_t);
            READFOURCC(DMUS_IO_TRACK_HEADER, ckid);
            READFOURCC(DMUS_IO_TRACK_HEADER, fccType);
        }
    };

    /// The DMUS_IO_VALID_START structure contains information about a valid
    /// start point in a segment that is to be cued to a rhythm.
    /// Used in the Marker Track List.
    struct DMUS_IO_VALID_START {
        /// Time of the start point.
        std::uint32_t mtTime;

        DMUS_IO_VALID_START() {}
        DMUS_IO_VALID_START(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_VALID_START, mtTime, std::uint32_t);
        }
    };

    /// The DMUS_IO_VERSION structure contains the version number of the data.
    /// Used in the version subchunk of various chunks.
    struct DMUS_IO_VERSION {
        /// High-order 32 bits of the version number.
        std::uint32_t dwVersionMS;

        /// Low-order 32 bits of the version number.
        std::uint32_t dwVersionLS;

        bool operator==(const DMUS_IO_VERSION& a) const {
            return dwVersionLS == a.dwVersionLS &&
                dwVersionMS == a.dwVersionMS;
        }

        DMUS_IO_VERSION() {}
        DMUS_IO_VERSION(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_VERSION, dwVersionMS, std::uint32_t);
            FIELDINIT(DMUS_IO_VERSION, dwVersionLS, std::uint32_t);
        }
    };

    /// The DMUS_IO_WAVE_HEADER structure describes streaming characteristics of a wave.
    /// It is used in the Wave Header Chunk of a WAV file.
    struct DMUS_IO_WAVE_HEADER {
        /// Time to read ahead in a streaming wave.
        std::int64_t rtReadAhead;
        std::uint32_t dwFlags;
    };

    /// The DMUS_IO_WAVE_ITEM_HEADER structure contains data for a wave sound in a Wave Track List.
    struct DMUS_IO_WAVE_ITEM_HEADER {
        /// Gain, in hundredths of a decibel. Must be a negative value.
        std::int32_t lVolume;

        /// Pitch offset, in hundredths of a semitone.
        std::int32_t lPitch;

        /// Variation flags. One bit is set for each variation this wave belongs to.
        std::uint32_t dwVariations;

        /// Start time, in reference time if the track is in clock time format; otherwise in music time.
        std::int64_t rtTime;

        /// Distance into wave to start playback, in reference time.
        std::int64_t rtStartOffset;

        /// Not used.
        std::int64_t rtReserved;

        /// Duration, in reference time if the track is in clock time format; otherwise in music time.
        std::int64_t rtDuration;

        /// Musical boundary where this belongs. Ignored if the track is in clock time format.
        std::uint32_t mtLogicalTime;

        /// Start point for a looping wave.
        std::uint32_t dwLoopStart;

        /// End point for a looping wave.
        std::uint32_t dwLoopEnd;
        std::uint32_t dwFlags;

        /// Amount by which volume can be randomized, in hundredths of a decibel.
        std::uint16_t wVolumeRange;

        /// Amount by which pitch can be randomized, in hundredths of a semitone.
        std::uint16_t wPitchRange;
    };

    /// The DMUS_IO_WAVE_PART_HEADER structure contains data for a Wave Track List.
    struct DMUS_IO_WAVE_PART_HEADER {
        /// Gain, in hundredths of a decibel, to apply to all waves in this wave part.
        /// This must be a negative value.
        std::int32_t lVolume;

        /// Performance channel of the part.
        std::uint32_t dwVariations;

        /// Performance channel of the part.
        std::uint32_t dwPChannel;

        /// Variation lock identifier. Parts with the same value in this member always play the same variation.
        /// A value of 0 means that the part plays its variations independently of all other parts.
        std::uint32_t dwLockToPart;

        /// Flags for managing how variations are chosen, in the lower four bits.
        /// See DMUS_VARIATIONT_TYPES.
        std::uint32_t dwFlags;

        /// Index for distinguishing multiple parts on the same performance channel.
        std::uint32_t dwIndex;
    };

    /// The DMUS_IO_WAVE_TRACK_HEADER structure contains data for a wave track in a Wave Track List.
    struct DMUS_IO_WAVE_TRACK_HEADER {
        /// Gain, hundredths of a decibel, to be applied to all waves.
        std::int32_t lVolume;
        std::uint32_t dwFlags;
    };

    /// The DMUS_IO_TEMPO_ITEM structure contains information about a tempo change in a track.
    /// Used in the Tempo Track Chunk.
    struct DMUS_IO_TEMPO_ITEM {
        /// Time of the tempo change.
        std::uint32_t lTime;

        /// Tempo, in beats per minute.
        double dblTempo;

        DMUS_IO_TEMPO_ITEM() {}
        DMUS_IO_TEMPO_ITEM(const std::uint8_t *data) {
            FIELDINIT(DMUS_IO_TEMPO_ITEM, lTime, std::uint32_t);

            dblTempo = *((double*)(data + offsetof(DMUS_IO_TEMPO_ITEM, dblTempo)));
        }
    };
#pragma pack(pop)
}

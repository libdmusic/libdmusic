#pragma once

#include <cstdint>

namespace DirectMusic {
    namespace DLS {
        #pragma pack(push, 1)
        struct MIDILOCALE {
            // Specifies the MIDI bank location.
            // Bits 0-6 are defined as MIDI CC32 and
            //  bits 8-14 are defined as MIDI CC0.
            // Bits 7 and 15-30 are reserved and
            //  should be written to zero.
            // If Bit 31 is equal to 1 then the instrument is a
            //  drum instrument; if equal to 0 then the instrument is a melodic
            //  instrument.
            std::uint32_t ulBank;

            // Specifies the MIDI Program Change (PC) value.
            // Bits 0-6 are defined as PC value and
            //  bits 7-31 are reserved and should be written to zero.
            std::uint32_t ulInstrument;
        };

        struct RGNRANGE {
            std::uint16_t usLow; /* Low Value of Range */
            std::uint16_t usHigh; /* High Value of Range*/
        };

        struct InstrumentHeader {
            // Specifies the count of regions for this instrument.
            std::uint32_t cRegions;

            // Specifies the MIDI locale for this instrument.
            MIDILOCALE Locale;
        };

        struct RegionHeader {
            // Specifies the key range for this region.
            RGNRANGE RangeKey;

            // Specifies the velocity range for this region.
            RGNRANGE RangeVelocity;

            // Specifies flag options for the synthesis of this region.
            std::uint16_t fusOptions;

            // Specifies the key group for a drum instrument.
            // Key group values allow multiple
            //  regions within a drum instrument to belong to
            //  the same “key group.” If a synthesis engine is
            //  instructed to play a note with a key group setting
            //  and any other notes are currently playing with this
            //  same key group, then the synthesis engine should turn
            //  off all notes with the same key group value as soon as
            //  possible.
            std::uint16_t usKeyGroup;
        };

        struct ConnectionBlock {
            // Specifies the source for the connection
            std::uint16_t usSource;

            // Specifies the control for the connection
            std::uint16_t usControl;

            // Specifies the destination for the connection
            std::uint16_t usDestination;

            // Specifies the transform for the connection
            std::uint16_t usTransform;

            // Specifies the scaling value used for the connection
            std::int32_t lScale;
        };

        struct WaveLink {
            // Specifies flag options for this wave link.
            std::uint16_t fusOptions;

            // Specifies a group number for samples which are phase locked.
            // All waves in a set of wave links with the same group are phase
            //  locked and follow the wave in the group with the
            //  F_WAVELINK_PHASE_MASTER flag set.
            // If a wave is not a member of a phase locked group,
            //  this value should be set to 0.
            std::uint16_t usPhaseGroup;

            // Specifies the channel placement of the file.
            // This is used to place mono sounds within a stereo pair
            //  or for multi-track placement.
            // Each bit position within the ulChannel field specifies
            //  a channel placement with bit 0 specifying a mono file
            //  or the left channel of a stereo file.
            // Bit 1 specifies the right channel of a stereo file.
            std::uint32_t ulChannel;

            // Specifies the 0 based index of the cue entry in the wave pool table.
            std::uint32_t ulTableIndex;
        };

        struct WavesampleLoop {
            // Specifies the size of the structure in bytes.
            std::uint32_t cbSize;

            // Specifies the loop type
            std::uint32_t ulLoopType;

            // Specifies the start point of the loop in samples as an absolute offset from the
            //  beginning of the data in the <data-ck> subchunk of the <wave-list> wave file chunk.
            std::uint32_t ulLoopStart;

            // Specifies the length of the loop in samples.
            std::uint32_t ulLoopLength;
        };

        struct Wavesample {
            // Specifies the size of the structure in bytes.
            // This size does not include the loop records.
            // This field is needed to distinguish the amount of data in the structure versus
            //  the list of loops and allow for additions to this structure in the future.
            // This cannot be determined from the chunk size.
            std::uint32_t cbSize;

            // Specifies the MIDI note which will replay the sample at original pitch.
            // This value ranges from 0 to 127 (a value of 60 represents Middle C, as defined in MIDI 1.0).
            std::uint16_t usUnityNote;

            // Specifies the tuning offset from the usUnityNote in 16 bit relative pitch.
            std::int16_t sFineTune;

            // Specifies the attenuation to be applied to this sample in 32 bit relative gain.
            std::int32_t lAttenuation;

            // Specifies flag options for the digital audio sample.
            std::uint32_t fulOptions;

            // Specifies the number (count) of <wavesample-loop> records that are contained in the <wsmp-ck> chunk.
            // The <wavesample-loop> records are stored immediately following the cSampleLoops data field.
            // One shot sounds will have the cSampleLoops field set to 0.
            // Looped sounds will have the cSampleLoops field set to 1.
            // Values greater than 1 are not yet defined.
            std::uint32_t cSampleLoops;
        };

        struct ArticulatorHeader {
            std::uint32_t cbSize;
            std::uint32_t cConnectionBlocks;
        };

        struct WaveFormat {
            // A number indicating the WAVE format category of the file.
            // The content of the <format-specific-fields> portion of the fmt chunk,
            //  and the interpretation of the waveform data, depend on this value.
            // DLS Level 1 only supports WAVE_FORMAT_PCM(0x0001) Microsoft Pulse Code Modulation(PCM) format
            std::uint16_t wFormatTag;

            // The number of channels represented in the waveform data,
            //  such as 1 for mono or 2 for stereo.DLS Level 1 supports only mono data(value = "1").
            std::uint16_t wChannels;

            // The sampling rate (in samples per second) at which each channel should be played.
            std::uint32_t dwSamplesPerSec;

            // The average number of bytes per second at which the waveform data should transferred.
            // Playback software can estimate the buffer size using this value.
            std::uint32_t dwAvgBytesPerSec;

            // The block alignment (in bytes) of the waveform data.
            // Playback software needs to process a multiple of
            //  wBlockAlign bytes of data at a time, so the value of
            //  wBlockAlign can be used for buffer alignment.
            std::uint16_t wBlockAlign;

            // Specifies the number of bits of data used to represent each sample of each channel.
            // If there are multiple channels, the sample size is the same for each channel.
            // DLS level 1 supports only 8 or 16 bit samples.
            std::uint16_t wBitsPerSample;
        };

        struct PoolTable {
            // Specifies the size of the structure in bytes.
            // This size does not include the poolcue records.
            // This field is needed to distinguish the amount of data in the structure versus
            //  the list of cues and allow for additions to this structure in the future.
            // This cannot be determined from the chunk size.
            std::uint32_t cbSize;

            // Specifies the number (count) of <poolcue> records that are contained in the <ptbl-ck> chunk.
            // The <poolcue> records are stored immediately following the cCues data field.
            std::uint32_t cCues;
        };
        #pragma pack(pop)
    }
}

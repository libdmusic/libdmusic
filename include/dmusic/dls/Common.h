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
        }

        struct RGNRANGE {
            std::uint16_t usLow; /* Low Value of Range */
            std::uint16_t usHigh; /* High Value of Range*/
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

        #pragma pack(pop)
    }
}

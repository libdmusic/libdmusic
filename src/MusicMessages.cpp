#include "MusicMessages.h"
#include "DummyPlayer.h"
#include <dmusic/Structs.h>
#include <dmusic/PlayingContext.h>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <exception>
#include <array>
#include <functional>

using namespace DirectMusic;

#define DMUSIC_CURVE_MESSAGE_SPACING 5

#define PI 3.14159265359

void MusicMessage::changeTempo(PlayingContext& ctx, double tempo) {
    ctx.m_tempo = tempo;
}

std::shared_ptr<InstrumentPlayer> MusicMessage::createInstrument(PlayingContext& ctx,
    std::uint8_t bank_lo, std::uint8_t bank_hi, std::uint8_t patch,
    const GUID& bandGuid, DirectMusic::DLS::DownloadableSound& dls, float volume, float pan) {
    return ctx.m_instrumentFactory(bank_lo, bank_hi, patch, bandGuid, dls, ctx.m_sampleRate, ctx.m_audioChannels, volume, pan);
}

std::shared_ptr<InstrumentPlayer> MusicMessage::createGMInstrument(PlayingContext& ctx,
    std::uint8_t bank_lo, std::uint8_t bank_hi, std::uint8_t patch,
    float volume, float pan) {
    if (ctx.m_gminstrumentFactory != nullptr) {
        return ctx.m_gminstrumentFactory(bank_lo, bank_hi, patch, ctx.m_sampleRate, ctx.m_audioChannels, volume, pan);
    } else {
        DLS::DownloadableSound snd;
        return std::make_shared<DummyPlayer>(bank_lo, bank_hi, patch, snd, ctx.m_sampleRate, ctx.m_audioChannels, volume, pan);
    }
}

void MusicMessage::setInstrument(PlayingContext& ctx, std::uint32_t channel, std::shared_ptr<InstrumentPlayer> instr) {
    ctx.m_performanceChannels[channel] = instr;
}


// From the Microsoft DX8 SDK docs
static int StoredRangeToActualRange(std::uint8_t bRange) {
    int nResult = 0;
    if (0 <= bRange && bRange <= 190) {
        nResult = bRange;
    } else if (191 <= bRange && bRange <= 212) {
        nResult = ((bRange - 190) * 5) + 190;
    } else if (213 <= bRange && bRange <= 232) {
        nResult = ((bRange - 212) * 10) + 300;
    } else // bRange > 232
    {
        nResult = ((bRange - 232) * 50) + 500;
    }
    return nResult;
}

enum class Subdivision {
    Grid,
    Beat,
    Measure
};

// Returns the nearest subdivision starting from time 'musicTime' according to 'timeSignature'
static std::uint32_t getNextGridSubdivision(std::uint32_t musicTime, Subdivision subd, const DMUS_IO_TIMESIG& timeSignature) {
    std::uint32_t divisionCoeff;
    if (timeSignature.bBeat == 0) {
        divisionCoeff = PlayingContext::PulsesPerQuarterNote / 64;
    } else if (timeSignature.bBeat > 4) {
        divisionCoeff = (PlayingContext::PulsesPerQuarterNote * 4) / timeSignature.bBeat;
    } else {
        divisionCoeff = PlayingContext::PulsesPerQuarterNote * (4 / timeSignature.bBeat);
    }
    divisionCoeff *= timeSignature.wGridsPerBeat;

    std::uint32_t nextGridDivision = (std::uint32_t)(ceil((double)musicTime / divisionCoeff));
    std::uint32_t nextBeatDivision = nextGridDivision * timeSignature.wGridsPerBeat;
    std::uint32_t nextMeasureDivision = nextBeatDivision * timeSignature.bBeatsPerMeasure;

    switch (subd) {
    case Subdivision::Beat:
        return nextBeatDivision * divisionCoeff;
    case Subdivision::Grid:
        return nextGridDivision * divisionCoeff;
    case Subdivision::Measure:
        return nextMeasureDivision * divisionCoeff;
    default:
        throw std::runtime_error("Invalid subdivision");
    }
}

static std::uint32_t getMeasureLength(const DMUS_IO_TIMESIG& timeSignature) {
    return (timeSignature.bBeatsPerMeasure * PlayingContext::PulsesPerQuarterNote * 4) / timeSignature.bBeat;
}

// From the Microsoft DX8 SDK docs
static std::int32_t getMusicOffset(std::uint32_t mtGridStart, std::int16_t nTimeOffset, const DMUS_IO_TIMESIG& TimeSig) {
    const std::uint32_t DMUS_PPQ = PlayingContext::PulsesPerQuarterNote;
    return nTimeOffset +
        (
        (mtGridStart / TimeSig.wGridsPerBeat) * ((DMUS_PPQ * 4) / TimeSig.bBeat)
            +
            (mtGridStart % TimeSig.wGridsPerBeat) * (((DMUS_PPQ * 4) / TimeSig.bBeat) / TimeSig.wGridsPerBeat)
            );
}

// Returns true if the specified degree is present in a certain scale, and puts the distance from the
// root note into offset. If not present, returns false.
static bool getOffsetFromScale(std::uint8_t degree, std::uint32_t scale, std::uint8_t* offset) {
    assert(offset != nullptr);

    // FIXME: There is probably a faster way to do this
    std::vector<int> degrees;
    for (int i = 0; i < 24; i++) {
        if (scale & (0x00000001 << i)) {
            degrees.push_back(i);
        }
    }

    if (degree < degrees.size()) {
        *offset = degrees[degree];
        return true;
    } else {
        *offset = degrees[degrees.size() - 1];
        return false;
    }
}

static bool MusicValueToMIDI(std::uint32_t chord, const std::vector<DMUS_IO_SUBCHORD>& subchords, DMUS_IO_STYLENOTE note, DMUS_IO_STYLEPART part, std::uint8_t* value) {
    assert(value != nullptr);

    if (note.bPlayModeFlags == DMUS_PLAYMODE_FIXED) {
        // In the original Gothic sountrack this is not used, but it might be useful for modding purposes
        *value = (std::uint8_t)(note.wMusicValue);
        return true;
    }

    assert(note.bPlayModeFlags == DMUS_PLAYMODE_CHORD_ROOT | DMUS_PLAYMODE_CHORD_INTERVALS | DMUS_PLAYMODE_SCALE_INTERVALS);
    // TODO: The subchord level should be obtained from the part reference,
    // but in Gothic's soundtrack it's always set to the first level.

    /*
    A chord (or a scale) has the following structure: the first 8 bits represent the
    root of the chord (scale), with the standard MIDI notation:
    0 -> C
    1 -> C#
    ....
    23 -> B

    The following 24 bits represent which notes are present in the chord, with each
    bit meaning a semitone from the root (the LSB). Example taken from G2:

    chord = 0x00AB5AB5
    0   0   A   B   5   A   B   5
    00000000101010110101101010110101
    |-------|-|-|-||-|-||-|-|-||-|-|
    C    W W W HW W HW W W HW W -----> C Major scale
    */
    DMUS_IO_SUBCHORD subchord;
    subchord.bChordRoot = 12;
    subchord.bScaleRoot = 0;
    subchord.dwChordPattern = 0x00000091;
    subchord.dwInversionPoints = 0xFFFFFFFF;
    subchord.dwLevels = 1;
    subchord.dwScalePattern = 0x00AB5AB5;

    if (subchords.size() > 0) {
        subchord = subchords[0];
    }

    /* Had to dig hard for this one: https://msdn.microsoft.com/en-us/library/ms898477.aspx
    Here is how to interpret wMusicValue:
    First nibble: octave (from -2 to 14 (??? I guess they meant -2 to 13) )
    Second nibble: chord tone (0 to 15)
    Third nibble: scale offset (0 to 15)
    Fourth nibble: accidentals (-8 to 7)
    */

    int octave = ((note.wMusicValue & 0xF000) >> 12);
    int chordTone = ((note.wMusicValue & 0x0F00) >> 8);
    int scaleTone = ((note.wMusicValue & 0x00F0) >> 4);

    // Explanation: the accidentals are represented as a two's complement 4bit value.
    // We first take only the last four bits from the word, then we shift it left
    // while keeping it unsigned, so that when we convert it into a signed byte
    // it has the correct sign. Then we divide by 16 to simulate an arithmetic
    // right shift of 4, to bring the value back into the correct range
    int accidentals = (std::int8_t)(note.wMusicValue & 0x000F);
    if (accidentals > 7) { accidentals = (accidentals - 16); }

    int noteValue = ((chord & 0xFF000000) >> 24) + 12 * octave;
    std::uint8_t chordOffset = 0;
    std::uint8_t scaleOffset = 0;
    if (getOffsetFromScale(chordTone, subchord.dwChordPattern, &chordOffset)) {
        noteValue += chordOffset;
        // Is it possible to not find chordTone?
        // } else if (getOffsetFromScale(chordTone, subchord.dwScalePattern, &scaleOffset)) {
        //     noteValue += scaleOffset;
    } else {
        TRACE_VERBOSE("Note not found: " << noteValue);
        return false;
    }

    // Start scale offset from resolved chordTone.
    // Let's draw an example:
    //      * chordTone is 2, scaleTone is 1
    //      * in normal chord chordTone 2 is resolved to 5th grade in scale
    //      * plus scaleTone 1 it makes 6th grade of the scale
    //      * depending on scale, it might be 8 semitones (minor) or 9 semitones (major)
    // So, scaleOffset depends on chord's grade
    // That works even if chord's grade doesn't fit into scale
    // Chord grade might be off of the scale, so if scaleOffset = 0, don't ruin it
    if (scaleTone && getOffsetFromScale(scaleTone, subchord.dwScalePattern >> chordOffset, &scaleOffset)) {
        noteValue += scaleOffset;
    }

    noteValue += accidentals;
    while (noteValue < 0) {
        noteValue += 12;
    }
    while (noteValue > 127) {
        noteValue -= 12;
    }

    *value = noteValue;
    return true;
}

static std::uint8_t getRandomVariation(const std::vector<StylePart>& parts) {
    int numVariations = 0;
    const std::array<std::uint32_t, 32>* choices = nullptr;

    for (const auto& part : parts) {
        int partialCount = 0;
        for (std::uint32_t variation : part.getHeader().dwVariationChoices) {
            if (variation & 0x0FFFFFFF) {
                partialCount++;
            }
        }
        if (numVariations < partialCount) {
            numVariations = partialCount;
            choices = &part.getHeader().dwVariationChoices;
        }
    }

    assert(choices != nullptr);
    std::uint8_t idx = std::rand() % numVariations;
    std::uint8_t j = -1;
    for (int i = 0; i < numVariations; i++) {
        if ((*choices)[i]) j++;
        if (j == idx)
            return i;
    }
    assert(false);
    return 0;
}


template<typename T>
static T lerp(float x, T start, T end) {
    return (1 - x) * start + x * end;
}

void MusicMessage::playPattern(PlayingContext& ctx) {
    TRACE("Playing pattern");
    ctx.m_patternMessageQueue = MessageQueue();
    for (const auto& kvpair : ctx.m_performanceChannels) {
        kvpair.second->allNotesOff();
    }

    if (ctx.m_primarySegment != nullptr && ctx.m_performanceChannels.size() > 0) {
        PlayingContext::Pattern pttn;
        if (ctx.getRandomPattern(*ctx.m_primarySegment, ctx.m_grooveLevel, &pttn)) {
            TRACE("Suitable pattern found: " << pttn.parts.size() << " parts");
            std::uint32_t patternLength = pttn.header.wNbrMeasures * getMeasureLength(pttn.header.timeSig);
            std::vector<StylePart> parts;
            for (const auto& pair : pttn.parts) {
                parts.push_back(pair.second);
            }
            std::uint32_t variation = 1 << getRandomVariation(parts);

            for (const auto& partTuple : pttn.parts) {
                const auto& partRef = partTuple.first;
                const auto& part = partTuple.second;
                const auto& header = part.getHeader();

                std::uint32_t PChannel = partRef.wLogicalPartID;
                if ((PChannel - 9) % 16 == 0) {
                    // Every 16th channel after the 10th is a percussion channel, we map all of them to the 10th
                    PChannel = 9;
                }

                for (const auto& note : part.getNotes()) {

                    if (!(note.dwVariation & variation)) continue;

                    std::uint8_t midiNote;
                    std::int32_t timeStart = getMusicOffset(note.mtGridStart, note.nTimeOffset, header.timeSig);

                    if (MusicValueToMIDI(ctx.m_chord, ctx.m_subchords, note, part.getHeader(), &midiNote)) {
                        std::uint32_t time = ctx.m_musicTime + timeStart;
                        auto noteOnMessage = std::make_shared<NoteOnMessage>(time, midiNote, note.bVelocity, 0, partRef.wLogicalPartID, PChannel);
                        assert(noteOnMessage != nullptr);
                        ctx.m_patternMessageQueue.push(noteOnMessage);

                        auto noteOffMessage = std::make_shared<NoteOffMessage>(time + note.mtDuration, midiNote, partRef.wLogicalPartID, PChannel);
                        assert(noteOffMessage != nullptr);
                        ctx.m_patternMessageQueue.push(noteOffMessage);
                    }
                }

                for (const auto& curve : part.getCurves()) {

                    if (!(curve.dwVariation & variation)) continue;

                    std::uint32_t timeStart = getMusicOffset(curve.mtGridStart, curve.nTimeOffset, header.timeSig);
                    std::uint32_t duration = curve.mtDuration;
                    if (curve.bEventType == DMUS_CURVET_CCCURVE) {
                        if (curve.nStartValue > 127 || curve.nEndValue > 127) continue;

                        float startValue = (float)curve.nStartValue / 127, endValue = (float)curve.nEndValue / 127;
                        auto control = (DirectMusic::Midi::Control)curve.bCCData;
                        std::array<std::function<float(float, float, float)>, 5> curves;

                        curves[DMUS_CURVES_LINEAR] = [](float x, auto start, auto end) { return lerp(x, start, end); };
                        curves[DMUS_CURVES_INSTANT] = [](float x, auto start, auto end) { return end; };
                        curves[DMUS_CURVES_EXP] = [](float x, auto start, auto end) { return lerp(x*x*x*x, start, end); };
                        curves[DMUS_CURVES_LOG] = [](float x, auto start, auto end) { return lerp(sqrtf(x), start, end); };
                        curves[DMUS_CURVES_SINE] = [](float x, auto start, auto end) { return lerp((sinf((x - 0.5) * PI) + 1) * 0.5, start, end); };

                        for (int i = 0; i < duration / DMUSIC_CURVE_MESSAGE_SPACING; i++) {
                            std::uint32_t offset = i * DMUSIC_CURVE_MESSAGE_SPACING;
                            float phase = (float)offset / duration;

                            assert(curve.bCurveShape < 5);
                            auto curveFunction = curves[curve.bCurveShape];

                            float value = curveFunction(phase, startValue, endValue);

                            auto msg = std::make_shared<ControlChangeMessage>(ctx.m_musicTime + timeStart + offset, partRef.wLogicalPartID, PChannel, control, value);
                            assert(msg != nullptr);
                            ctx.m_patternMessageQueue.push(msg);
                        }
                    }
                }
            }

            auto patternEndMessage = std::make_shared<PatternEndMessage>(ctx.m_musicTime + patternLength);
            ctx.m_patternMessageQueue.push(patternEndMessage);
        } else {
            TRACE("No suitable pattern found");
        }
    }
}

void MusicMessage::setGrooveLevel(PlayingContext& ctx, std::uint8_t level) {
    ctx.m_grooveLevel = level;

    playPattern(ctx);
}

const std::map<std::uint32_t, std::shared_ptr<InstrumentPlayer>>& MusicMessage::getChannels(PlayingContext& ctx) {
    return ctx.m_performanceChannels;
}

void MusicMessage::changeChord(PlayingContext& ctx, std::uint32_t chord, const std::vector<DMUS_IO_SUBCHORD>& subchords) {
    ctx.m_chord = chord;
    ctx.m_subchords = std::move(subchords);
}

void MusicMessage::enqueueNextSegment(PlayingContext& ctx) {
    if (ctx.m_nextSegment != nullptr) {
        TRACE("New segment enqueued");
        ctx.enqueueSegment(ctx.m_nextSegment);
        ctx.m_primarySegment = std::move(ctx.m_nextSegment);
        ctx.m_nextSegment = nullptr;
    } else if (ctx.m_primarySegment != nullptr) {
        TRACE("Looping current segment");
        ctx.enqueueSegment(ctx.m_primarySegment);
    }
}

bool MusicMessage::isNextSegmentAvailable(PlayingContext& ctx) {
    return ctx.m_nextSegment != nullptr;
}

SegmentTiming MusicMessage::getNextSegmentTiming(PlayingContext& ctx) {
    return ctx.m_nextSegmentTiming;
}

void TempoChangeMessage::Execute(PlayingContext& ctx) {
    TRACE("Tempo change: " << m_tempo);
    this->changeTempo(ctx, m_tempo);
}

BandChangeMessage::BandChangeMessage(PlayingContext& ctx, std::uint32_t time, const BandForm& form)
    : MusicMessage(time) {
    for (const auto& instr : form.getInstruments()) {
        const auto& header = instr.getHeader();
        const auto ref = instr.getReference();

        std::uint8_t bankHi = (header.dwPatch & 0x00FF0000) >> 0x10;
        std::uint8_t bankLo = (header.dwPatch & 0x0000FF00) >> 0x8;
        std::uint8_t patch = (header.dwPatch & 0x000000FF);
        float volume = (header.bVolume * header.bVolume) / (127.0 * 127.0);
        float pan = ((float)(header.bPan) - 63.0f) / 64.0f;

        if (ref != nullptr) {
            auto dls = ctx.loadInstrumentCollection(ref->getGuid(), form.getGuid(), ref->getFile());

            assert(dls != nullptr);
            instruments[header.dwPChannel] = createInstrument(ctx, bankLo, bankHi, patch, form.getGuid(), *dls, volume, pan);
        } else {
            // The instrument is to be played from a standard GM preset
            instruments[header.dwPChannel] = createGMInstrument(ctx, bankLo, bankHi, patch, volume, pan);
        }
    }
}

void BandChangeMessage::Execute(PlayingContext& ctx) {
    TRACE("Band change");
    for (const auto& kvpair : instruments) {
        setInstrument(ctx, kvpair.first, kvpair.second);
    }
}

void GrooveLevelMessage::Execute(PlayingContext& ctx) {
    if (m_range == 0) {
        TRACE("Groove change: " << (int)m_level);
        setGrooveLevel(ctx, m_level);
    } else {
        std::int8_t offset = (std::rand() % m_range) - (m_range / 2);
        std::uint8_t newLevel = m_level - offset;
        TRACE("Groove change: " << (int)newLevel);
        setGrooveLevel(ctx, newLevel);
    }
}

void ChordMessage::Execute(PlayingContext& ctx) {
    TRACE("Chord change");
    changeChord(ctx, this->m_chord, this->m_subchords);
}

static std::shared_ptr<InstrumentPlayer> findChannel(const std::map<std::uint32_t, std::shared_ptr<InstrumentPlayer>>& channels, std::uint32_t channel, std::uint32_t channelAlt) {
    if (channels.find(channel) != channels.end()) {
        return channels.at(channel);
    } else if (channels.find(channelAlt) != channels.end()) {
        return channels.at(channelAlt);
    } else {
        return nullptr;
    }
}

void NoteOnMessage::Execute(PlayingContext& ctx) {
    TRACE_VERBOSE("Note on");
    const auto& channels = getChannels(ctx);
    std::shared_ptr<InstrumentPlayer> player = nullptr;
    if (player = findChannel(channels, m_channel, m_channelAlt)) {
        if (m_velRange == 0) {
            player->noteOn(m_note, m_vel);
        } else {
            std::int8_t offset = (std::rand() % m_velRange) - (m_velRange / 2);
            player->noteOn(m_note, m_vel - offset);
        }
    }
    assert(player != nullptr);
}

void NoteOffMessage::Execute(PlayingContext& ctx) {
    TRACE_VERBOSE("Note off");
    const auto& channels = getChannels(ctx);
    std::shared_ptr<InstrumentPlayer> player = nullptr;
    if (player = findChannel(channels, m_channel, m_channelAlt)) {
        player->noteOff(m_note, 0);
    }
    assert(player != nullptr);
}

void SegmentEndMessage::Execute(PlayingContext& ctx) {
    TRACE("Segment end");
    enqueueNextSegment(ctx);
}

void PatternEndMessage::Execute(PlayingContext& ctx) {
    TRACE("Pattern end");
    if (isNextSegmentAvailable(ctx) && getNextSegmentTiming(ctx) == SegmentTiming::Pattern)
        enqueueNextSegment(ctx);
    playPattern(ctx);
}

void ControlChangeMessage::Execute(PlayingContext& ctx) {
    TRACE("Control change");
    const auto& channels = getChannels(ctx);
    std::shared_ptr<InstrumentPlayer> player = nullptr;
    if (player = findChannel(channels, m_channel, m_channelAlt)) {
        player->controlChange(m_control, m_value);
    }
    assert(player != nullptr);
}
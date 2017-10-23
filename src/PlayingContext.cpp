#include <dmusic/PlayingContext.h>
#include <dmusic/Tracks.h>
#include "MusicMessages.h"
#include <assert.h>
#include <math.h>
#include <bitset>

using namespace DirectMusic;

enum class Subdivision {
    Grid,
    Beat,
    Measure
};

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

// Returns the nearest subdivision starting from time 'musicTime' according to 'timeSignature'
static std::uint32_t getNextGridSubdivision(std::uint32_t musicTime, Subdivision subd, DMUS_IO_TIMESIG timeSignature) {
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
    }
}

// From the Microsoft DX8 SDK docs
static std::uint32_t getMusicOffset(std::uint32_t mtGridStart, std::int16_t nTimeOffset, DMUS_IO_TIMESIG TimeSig) {
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
    DMUS_IO_SUBCHORD subchord = subchords[0];

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
    std::cout << (int)noteValue << "\n"; 
    return true;
}

void PlayingContext::renderBlock(std::int16_t *data, std::uint32_t count, float volume) noexcept {
    m_queueMutex.lock();

    double pulsesPerSecond = PulsesPerQuarterNote * (m_tempo / 60);
    double pulsesPerSample = pulsesPerSecond / m_sampleRate;

    std::uint32_t offset = 0;
    while (offset < count) {
        // Segment looping, we have to wait for a band to be loaded
        if (m_segmentQueue.empty() && m_primarySegment != nullptr && m_performanceChannels.size() > 0 && m_subchords.size() > 0) {
            if (m_primarySegment->numLoops > 0 || m_primarySegment->infiniteLoop) {
                Pattern pttn;
                if (m_primarySegment->getRandomPattern(m_grooveLevel, &pttn)) {
                    for (const auto& partTuple : pttn.parts) {
                        const auto& partRef = partTuple.first;
                        const auto& part = partTuple.second;

                        /*if (partRef.wLogicalPartID == 0) */{
                            for (const auto& note : part.getNotes()) {
                                std::uint8_t midiNote;
                                if (MusicValueToMIDI(m_chord, m_subchords, note, part.getHeader(), &midiNote)) {
                                    std::uint32_t timeStart = getMusicOffset(note.mtGridStart, note.nTimeOffset, part.getHeader().timeSig);
                                    auto noteOnMessage = std::make_shared<NoteOnMessage>(m_musicTime + timeStart, midiNote, note.bVelocity, 0, partRef.wLogicalPartID);
                                    assert(noteOnMessage != nullptr);
                                    m_segmentQueue.push(noteOnMessage);

                                    auto noteOffMessage = std::make_shared<NoteOffMessage>(m_musicTime + timeStart + note.mtDuration, midiNote, partRef.wLogicalPartID);
                                    assert(noteOffMessage != nullptr);
                                    m_segmentQueue.push(noteOffMessage);
                                }
                            }
                        }
                    }

                    if (!m_primarySegment->infiniteLoop) {
                        m_primarySegment->numLoops--;
                    }
                }
            } else {
                m_primarySegment = nullptr;
            }
        }

        std::shared_ptr<MusicMessage> nextMessage = nullptr;
        bool messageIsfromSegment = false;

        if (!m_segmentQueue.empty()) {
            nextMessage = m_segmentQueue.top();
            messageIsfromSegment = true;
        }

        if (!m_messageQueue.empty()) {
            if (nextMessage == nullptr || m_messageQueue.top()->getMessageTime() < nextMessage->getMessageTime()) {
                nextMessage = m_messageQueue.top();
                messageIsfromSegment = false;
            }
        }

        if (nextMessage == nullptr) {
            goto fill_buffer;
        } else {
            pulsesPerSecond = PulsesPerQuarterNote * (m_tempo / 60);
            pulsesPerSample = pulsesPerSecond / (m_sampleRate * m_audioChannels);
            std::uint32_t nextMessageTimeOffset = nextMessage->getMessageTime() - m_musicTime;
            assert(nextMessageTimeOffset >= 0);
            std::uint32_t nextMessageTimeOffsetInSamples = (std::uint32_t)(nextMessageTimeOffset / pulsesPerSample);
            if (nextMessageTimeOffsetInSamples + offset > count) {
                goto fill_buffer;
            } else {
                for (const auto& channel : m_performanceChannels) {
                    {
                        const auto& player = channel.second;
                        player->renderBlock(data + offset, nextMessageTimeOffsetInSamples, volume);
                    }
                }
                offset += nextMessageTimeOffsetInSamples;
                m_musicTime += nextMessageTimeOffset;
                if (messageIsfromSegment) {
                    m_segmentQueue.pop();
                } else {
                    m_messageQueue.pop();
                }
                nextMessage->Execute(*this);
            }
        }
    }

fill_buffer:
    // There are no more messages to interpret in this block, we just
    // process the already-playing instruments
    int remainingSamples = count - offset;
    if (remainingSamples > 0) {
        for (const auto& channel : m_performanceChannels) {
            {
                const auto& player = channel.second;
                player->renderBlock(data + offset, remainingSamples);
            }
        }
        m_musicTime += (remainingSamples * pulsesPerSample);
    }

    m_queueMutex.unlock();
}

void PlayingContext::playSegment(const SegmentForm& segment/*, DMUS_SEGF_FLAGS flags, std::int64_t startTime*/) {
    m_queueMutex.lock();
    for (const auto& track : segment.getTracks()) {
        const auto& header = track.getHeader();
        std::string ckid = std::string(header.ckid),
            fccType = std::string(header.fccType);

        ckid.resize(4);
        fccType.resize(4);

        if (ckid == "tetr") {
            auto tempoTrack = std::static_pointer_cast<TempoTrack>(track.getData());
            for (const auto& item : tempoTrack->getItems()) {
                auto message = std::make_shared<TempoChangeMessage>(item.lTime, item.dblTempo);
                assert(message != nullptr);
                m_messageQueue.push(message);
            }
        } else if (ckid == "cmnd") {
            auto commandTrack = std::static_pointer_cast<CommandTrack>(track.getData());
            for (const auto& command : commandTrack->getCommands()) {
                auto message = std::make_shared<GrooveLevelMessage>(command.mtTime, command.bGrooveLevel, command.bGrooveRange);
                assert(message != nullptr);
                m_messageQueue.push(message);
            }
        } else if (*header.ckid == 0 && fccType == "sttr") {
            auto styleTrack = std::static_pointer_cast<StyleTrack>(track.getData());
            for (const auto& style : styleTrack->getStyles()) {
                const std::uint16_t timestamp = style.first;
                const ReferenceList refs = style.second;

                std::string styleFile = refs.getFile();
                auto styleForm = loadStyle(std::string(styleFile.begin(), styleFile.end()));
                assert(styleForm != nullptr);
                m_primarySegment = std::make_unique<Segment>();
                m_primarySegment->numLoops = segment.getHeader().dwRepeats;
                m_primarySegment->infiniteLoop = false;
                std::map<GUID, StylePart, GuidComparer> parts;
                for (const auto& part : styleForm->getParts()) {
                    parts[part.getHeader().guidPartID] = part;
                }

                for (const auto& pattern : styleForm->getPatterns()) {
                    Pattern pttn;
                    pttn.header = pattern.getHeader();
                    for (const auto& partRefTuple : pattern.getPartReferences()) {
                        const auto& partRef = partRefTuple.first;
                        const auto& info = partRefTuple.second;

                        const auto& partGuid = partRef.guidPartID;

                        assert(parts.find(partGuid) != parts.end());
                        StylePart part = parts[partGuid];
                        pttn.parts.push_back(std::make_pair(partRef, part));
                    }

                    m_primarySegment->patterns.push_back(pttn);
                }

                // Load the style's band
                for (const auto& band : styleForm->getBands()) {
                    auto message = std::make_shared<BandChangeMessage>(*this, 0, band);
                    m_messageQueue.push(message);
                }

                // Load the style's tempo
                auto message = std::make_shared<TempoChangeMessage>(0, styleForm->getHeader().dblTempo);
                m_messageQueue.push(message);
            }
        } else if (*header.ckid == 0 && fccType == "DMBT") {
            auto bandTrack = std::static_pointer_cast<BandTrack>(track.getData());
            for (const auto& band : bandTrack->getBands()) {
                DMUS_IO_BAND_ITEM_HEADER2 header = band.first;
                BandForm bandForm = band.second;

                auto message = std::make_shared<BandChangeMessage>(*this, header.lBandTimePhysical, bandForm);
                m_messageQueue.push(message);
            }
        } else if (*header.ckid == 0 && fccType == "cord") {
            auto chordTrack = std::static_pointer_cast<ChordTrack>(track.getData());
            for (const auto& chord : chordTrack->getChords()) {
                const auto& chordHeader = chord.first;
                const auto& chordBody = chord.second;
                auto message = std::make_shared<ChordMessage>(chordHeader.mtTime, chordTrack->getHeader(), chordBody);
                m_messageQueue.push(message);
            }
        }
    }
    m_queueMutex.unlock();
}

bool PlayingContext::Segment::getRandomPattern(std::uint8_t grooveLevel, Pattern* output) const {
    std::vector<int> suitablePatterns;
    for (int i = 0; i < patterns.size(); i++) {
        const auto& pattern = patterns[i];
        if (pattern.header.bGrooveBottom <= grooveLevel && pattern.header.bGrooveTop >= grooveLevel) {
            suitablePatterns.push_back(i);
        }
    }

    if (suitablePatterns.size() == 0) {
        return false;
    } else if (suitablePatterns.size() == 1) {
        *output = patterns[suitablePatterns[0]];
        return true;
    } else {
        int idx = rand();
        *output = patterns[suitablePatterns[idx % suitablePatterns.size()]];
        return true;
    }
}
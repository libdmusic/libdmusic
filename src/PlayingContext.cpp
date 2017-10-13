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
static std::uint32_t getMusicOffset(std::uint32_t mtGridStart, std::uint32_t nTimeOffset, DMUS_IO_TIMESIG TimeSig) {
    const std::uint32_t DMUS_PPQ = PlayingContext::PulsesPerQuarterNote;
    return nTimeOffset +
        (
        (mtGridStart / TimeSig.wGridsPerBeat) * ((DMUS_PPQ * 4) / TimeSig.bBeat)
            +
            (mtGridStart % TimeSig.wGridsPerBeat) * (((DMUS_PPQ * 4) / TimeSig.bBeat) / TimeSig.wGridsPerBeat)
            );
}

static std::uint8_t getNoteInScale(std::uint32_t chord, DMUS_IO_STYLENOTE note) {
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
    std::uint8_t root = ((chord & 0xFF000000) >> 24);

}

void PlayingContext::renderBlock(std::int16_t *data, std::uint32_t count, float volume) noexcept {
    m_queueMutex.lock();
    double pulsesPerSecond = PulsesPerQuarterNote * m_tempo;
    double pulsesPerSample = pulsesPerSecond / m_sampleRate;

    if (m_messageQueue.empty() && m_primarySegment != nullptr) {
        if (m_primarySegment->numLoops > 0 || m_primarySegment->infiniteLoop) {
            Pattern* pttn = m_primarySegment->getRandomPattern(m_grooveLevel);
            if (pttn != nullptr) {
                /*for (const auto& note : pttn->notes) {
                std::uint32_t timeOffset = getMusicOffset(note.mtDuration, note.nTimeOffset, pttn->timeSignature);
                // TODO: Implement bDurRange, bTimeRange and bVelRange
                }*/

                if (!m_primarySegment->infiniteLoop) {
                    m_primarySegment->numLoops--;
                }
            }
        } else {
            m_primarySegment = nullptr;
        }
    }

    if (!m_messageQueue.empty()) {
        auto nextMessage = m_messageQueue.top();

        double nextMessageTimeOffset = nextMessage->getMessageTime() - m_musicTime;
        if (nextMessageTimeOffset / pulsesPerSample > count) {
            // There are no messages to interpret in this block, we just
            // process the already-playing instruments
            for (const auto& channel : m_performanceChannels) {
                const auto& player = channel.second;
                player->renderBlock(data, count);
            }
            m_musicTime += (count * pulsesPerSample);
        } else {
            std::uint32_t offset = 0;
            do {
                // We process the audio before the message, then we send the message
                double nextMessageTimeOffsetInSamples = nextMessageTimeOffset / pulsesPerSample;
                for (const auto& channel : m_performanceChannels) {
                    const auto& player = channel.second;
                    player->renderBlock(data + offset, (std::uint32_t)nextMessageTimeOffsetInSamples);
                }
                offset += (std::uint32_t)nextMessageTimeOffsetInSamples;
                m_musicTime += nextMessageTimeOffset;
                if (!m_messageQueue.empty()) m_messageQueue.pop();
                nextMessage->Execute(*this);
                if (m_messageQueue.empty()) {
                    nextMessage = nullptr;
                } else {
                    nextMessage = m_messageQueue.top();
                    nextMessageTimeOffset = nextMessage->getMessageTime() - m_musicTime;
                }
            } while (offset < count);
        }
    } else {
        // There are no messages to interpret in this block, we just
        // process the already-playing instruments
        for (const auto& channel : m_performanceChannels) {
            const auto& player = channel.second;
            player->renderBlock(data, count);
        }
        m_musicTime += (count * pulsesPerSample);
    }
    m_queueMutex.unlock();
}

void PlayingContext::playSegment(const SegmentForm& segment/*, DMUS_SEGF_FLAGS flags, std::int64_t startTime*/) {
    m_queueMutex.lock();
    for (const auto& track : segment.getTracks()) {
        const auto& header = track.getHeader();
        std::string ckid = std::string(header.ckid),
            fccType = std::string(header.fccType);
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
        } else if (ckid == "" && fccType == "sttr") {
            auto styleTrack = std::static_pointer_cast<StyleTrack>(track.getData());
            for (const auto& style : styleTrack->getStyles()) {
                const std::uint16_t timestamp = std::get<0>(style);
                const ReferenceList refs = std::get<1>(style);

                std::wstring styleFile = refs.getFile();
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
                    std::cout << "--------------> " << pattern.getInfo().getName() << " <--------------\n";
                    pttn.header = pattern.getHeader();
                    for (const auto& partRefTuple : pattern.getPartReferences()) {
                        const auto& partRef = std::get<0>(partRefTuple);
                        const auto& info = std::get<1>(partRefTuple);

                        const auto& partGuid = partRef.guidPartID;

                        assert(parts.find(partGuid) != parts.end());
                        StylePart part = parts[partGuid];
                        pttn.parts.push_back(part);
                        std::cout << " -- " << info.getName() << " --\n";
                        for (const auto& note : part.getNotes()) {
                            std::cout << note.bPlayModeFlags << " ";
                            if (note.bPlayModeFlags == DMUS_PLAYMODE_FIXED) {
                                std::cout << "FIXED: ";
                            } else if (note.bPlayModeFlags == DMUS_PLAYMODE_NONE) {
                                std::cout << "DEFAULT: ";
                            } else {
                                std::cout << "OTHER: ";
                            }
                            std::cout << note.wMusicValue << " " << std::bitset<32>(note.dwVariation) << "\n";
                        }
                    }

                    m_primarySegment->patterns.push_back(pttn);
                }
            }
        } else if (ckid == "" && fccType == "DMBT") {
            auto bandTrack = std::static_pointer_cast<BandTrack>(track.getData());
            for (const auto& band : bandTrack->getBands()) {
                DMUS_IO_BAND_ITEM_HEADER2 header = std::get<0>(band);
                BandForm bandForm = std::get<1>(band);

                auto message = std::make_shared<BandChangeMessage>(*this, header.lBandTimePhysical, bandForm);
                m_messageQueue.push(message);
            }
        } else if (ckid == "" && fccType == "cord") {
            auto chordTrack = std::static_pointer_cast<ChordTrack>(track.getData());
            for (const auto& chord : chordTrack->getChords()) {
                const auto& chordHeader = std::get<0>(chord);
                const auto& chordBody = std::get<1>(chord);
                auto message = std::make_shared<ChordMessage>(chordHeader.mtTime, chordTrack->getHeader(), chordBody);
                m_messageQueue.push(message);
            }
        }
    }
    m_queueMutex.unlock();
}

PlayingContext::Pattern* PlayingContext::Segment::getRandomPattern(std::uint8_t grooveLevel) const {
    std::vector<Pattern> suitablePatterns;
    for (const auto& pattern : patterns) {
        if (pattern.header.bGrooveBottom <= grooveLevel && pattern.header.bGrooveTop >= grooveLevel) {
            suitablePatterns.push_back(pattern);
        }
    }

    if (suitablePatterns.size() == 0) { return nullptr; } else if (suitablePatterns.size() == 1) { return &suitablePatterns[0]; } else {
        int idx = rand();
        return &suitablePatterns[idx % suitablePatterns.size()];
    }
}
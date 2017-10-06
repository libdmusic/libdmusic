#include <dmusic/PlayingContext.h>
#include <dmusic/Tracks.h>
#include "MusicMessages.h"
#include <assert.h>

using namespace DirectMusic;

enum class Subdivision {
    Grid,
    Beat,
    Measure
};

static std::uint32_t getNextGridSubdivision(std::uint32_t musicTime, Subdivision subd, DMUS_IO_TIMESIG timeSignature) {
    std::uint32_t divisionCoeff;
    if (timeSignature.bBeat == 0) {
        divisionCoeff = PlayingContext::PulsesPerQuarterNote / 64;
    } else if(timeSignature.bBeat > 4) {
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

void PlayingContext::renderBlock(std::int16_t *data, std::uint32_t count, float volume) noexcept {
    m_queueMutex.lock();
    double pulsesPerSecond = PulsesPerQuarterNote * m_tempo;
    double pulsesPerSample = pulsesPerSecond / m_sampleRate;
    auto nextMessage = m_messageQueue.top();
    m_queueMutex.unlock();

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
        do {
            // We process the audio before the message, then we send the message
            double nextMessageTimeOffsetInSamples = nextMessageTimeOffset / pulsesPerSample;
            for (const auto& channel : m_performanceChannels) {
                const auto& player = channel.second;
                player->renderBlock(data, (std::uint32_t)nextMessageTimeOffsetInSamples);
            }

            m_queueMutex.lock();
            m_musicTime += nextMessageTimeOffset;
            m_messageQueue.pop();
            nextMessage->Execute(*this);
            nextMessage = m_messageQueue.top();
            nextMessageTimeOffset = nextMessage->getMessageTime() - m_musicTime;
            m_queueMutex.unlock();
        } while (true);
    }
}

void PlayingContext::playSegment(const SegmentForm& segment/*, DMUS_SEGF_FLAGS flags, std::int64_t startTime*/) {
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
                std::map<GUID, StylePart, GuidComparer> parts;
                for (const auto& part : styleForm->getParts()) {
                    parts[part.getHeader().guidPartID] = part;
                }

                for (const auto& pattern : styleForm->getPatterns()) {
                    Pattern pttn;
                    pttn.grooveLower = pattern.getHeader().bGrooveBottom;
                    pttn.grooveUpper = pattern.getHeader().bGrooveTop;
                    pttn.performanceChannel = pattern.getPartReference().dwPChannel;

                    const auto& partGuid = pattern.getPartReference().guidPartID;
                    
                    assert(parts.find(partGuid) != parts.end());
                    StylePart part = parts[partGuid];
                    pttn.notes = part.getNotes();
                    pttn.timeSignature = part.getHeader().timeSig;
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
            auto message = std::make_shared<ChordMessage>(chordTrack->getChord().mtTime, chordTrack->getHeader());
            m_messageQueue.push(message);
        }
    }
}

PlayingContext::Pattern* PlayingContext::Segment::getRandomPattern(std::uint8_t grooveLevel) const {
    std::vector<Pattern> suitablePatterns;
    for (const auto& pattern : patterns) {
        if (pattern.grooveLower <= grooveLevel && pattern.grooveUpper >= grooveLevel) {
            suitablePatterns.push_back(pattern);
        }
    }

    if (suitablePatterns.size() == 0) { return nullptr; }
    else if (suitablePatterns.size() == 1) { return &suitablePatterns[0]; }
    else {
        int idx = rand();
        return &suitablePatterns[idx % suitablePatterns.size()];
    }
}
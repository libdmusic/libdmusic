#include <dmusic/PlayingContext.h>
#include <dmusic/Tracks.h>
#include "MusicMessages.h"
#include <assert.h>

using namespace DirectMusic;

void PlayingContext::renderBlock(std::int16_t *data, std::uint32_t count, float volume) noexcept {
    /*m_queueMutex.lock();
    double pulsesPerSecond = PulsesPerQuarterNote * m_tempo;
    double pulsesPerSample = pulsesPerSample / m_sampleRate;
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
    m_musicTime += (blockDuration * pulsesPerSecond);
    } else {
    do {
    // We process the audio before the message, then we send the message
    double nextMessageTimeOffsetInSamples = nextMessageTimeOffset / pulsesPerSample;
    for (const auto& channel : m_performanceChannels) {
    const auto& player = channel.second;
    player->renderBlock(data, (std::uint32_t)nextMessageTimeOffsetInSamples);
    }

    switch (nextMessage->getMessageType()) {
    case MessageType::ChannelPressure:
    std::shared_ptr<ChannelPressureMessage> cpm = std::dynamic_pointer_cast(nextMessage);
    cpm->
    break;
    }

    } while(???)
    }*/
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
                auto style = loadStyle(std::string(styleFile.begin(), styleFile.end()));
                assert(style != nullptr);
                m_primarySegment = std::make_unique<Segment>();
                std::map<GUID, StylePart, GuidComparer> parts;
                for (const auto& part : style->getParts()) {
                    parts[part.getHeader().guidPartID] = part;
                }

                for (const auto& pattern : style->getPatterns()) {
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

    if (suitablePatterns.size() == 0) return nullptr;
    else if (suitablePatterns.size() == 1) return &suitablePatterns[0];
    else {
        int idx = rand();
        return &suitablePatterns[idx % suitablePatterns.size()];
    }
}
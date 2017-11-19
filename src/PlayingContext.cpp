#include <dmusic/PlayingContext.h>
#include <dmusic/Tracks.h>
#include "MusicMessages.h"
#include <cassert>
#include <cmath>
#include <bitset>

using namespace DirectMusic;

void PlayingContext::renderBlock(std::int16_t *data, std::uint32_t count, float volume) noexcept {
    m_queueMutex.lock();

    double pulsesPerSecond = PulsesPerQuarterNote * (m_tempo / 60);
    double pulsesPerSample = pulsesPerSecond / m_sampleRate;

    MusicMessageComparer comparer;

    std::uint32_t offset = 0;
    while (offset < count) {
        std::shared_ptr<MusicMessage> nextMessage = nullptr;
        bool messageIsfromPattern = false;

        if (!m_patternMessageQueue.empty()) {
            nextMessage = m_patternMessageQueue.top();
            messageIsfromPattern = true;
        }

        if (!m_messageQueue.empty()) {
            if (nextMessage == nullptr || comparer(nextMessage, m_messageQueue.top())) {
                nextMessage = m_messageQueue.top();
                messageIsfromPattern = false;
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
            if (nextMessageTimeOffsetInSamples % m_audioChannels != 0) {
                nextMessageTimeOffsetInSamples++;
                nextMessageTimeOffset = (std::uint32_t)(nextMessageTimeOffsetInSamples * pulsesPerSample);
            }

            if (nextMessageTimeOffsetInSamples + offset > count) {
                goto fill_buffer;
            } else {
                bool first = true;
                for (const auto& channel : m_performanceChannels) {
                    {
                        const auto& player = channel.second;
                        player->renderBlock(data + offset, nextMessageTimeOffsetInSamples, volume, !first);
                        first = false;
                    }
                }
                offset += nextMessageTimeOffsetInSamples;
                m_musicTime += nextMessageTimeOffset;
                if (messageIsfromPattern) {
                    m_patternMessageQueue.pop();
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
        bool first = true;
        for (const auto& channel : m_performanceChannels) {
            {
                const auto& player = channel.second;
                player->renderBlock(data + offset, remainingSamples, volume, !first);
                first = false;
            }
        }
        m_musicTime += (remainingSamples * pulsesPerSample);
    }

    m_queueMutex.unlock();
}

void PlayingContext::enqueueSegment(const std::shared_ptr<SegmentInfo>& segment) {
    assert(segment != nullptr);
    TRACE("Segment enqueued");
    m_messageQueue = MessageQueue();
    m_messageQueue.push(std::make_shared<TempoChangeMessage>(m_musicTime, segment->initialTempo));
    for (const auto& message : segment->messages) {
        m_messageQueue.push(message->Clone(message->getMessageTime() + m_musicTime));
    }
}

// Loads tempo change information into the message vector
static void loadTempoTrack(const TrackForm& track, std::vector<std::shared_ptr<MusicMessage>>& messageVector) {
    auto tempoTrack = std::static_pointer_cast<TempoTrack>(track.getData());
    for (const auto& item : tempoTrack->getItems()) {
        auto message = std::make_shared<TempoChangeMessage>(item.lTime, item.dblTempo);
        assert(message != nullptr);
        messageVector.push_back(message);
    }
}

// Loads commands information (for now only groove level changes) into the message vector
static void loadCommandTrack(const TrackForm& track, std::vector<std::shared_ptr<MusicMessage>>& messageVector) {
    auto commandTrack = std::static_pointer_cast<CommandTrack>(track.getData());
    for (const auto& command : commandTrack->getCommands()) {
        auto message = std::make_shared<GrooveLevelMessage>(command.mtTime, command.bGrooveLevel, command.bGrooveRange);
        assert(message != nullptr);
        messageVector.push_back(message);
    }
}

// Loads band change information into the message vector
static void loadBandTrack(const TrackForm& track, std::vector<std::shared_ptr<MusicMessage>>& messageVector, PlayingContext& ctx) {
    auto bandTrack = std::static_pointer_cast<BandTrack>(track.getData());
    for (const auto& band : bandTrack->getBands()) {
        DMUS_IO_BAND_ITEM_HEADER2 header = band.first;
        BandForm bandForm = band.second;

        auto message = std::make_shared<BandChangeMessage>(ctx, header.lBandTimePhysical, bandForm);
        messageVector.push_back(message);
    }
}

// Loads chord change information into the message vector
static void loadChordTrack(const TrackForm& track, std::vector<std::shared_ptr<MusicMessage>>& messageVector) {
    auto chordTrack = std::static_pointer_cast<ChordTrack>(track.getData());
    for (const auto& chord : chordTrack->getChords()) {
        const auto& chordHeader = chord.first;
        const auto& chordBody = chord.second;
        auto message = std::make_shared<ChordMessage>(chordHeader.mtTime, chordTrack->getHeader(), std::move(chordBody));
        messageVector.push_back(message);
    }
}

std::shared_ptr<SegmentInfo> PlayingContext::prepareSegment(const SegmentForm& segment) {
    TRACE("Preparing segment");
    auto newSegment = std::make_shared<SegmentInfo>();
    newSegment->numLoops = segment.getHeader().dwRepeats;
    newSegment->infiniteLoop = false;
    newSegment->length = segment.getHeader().mtLength;
    newSegment->messages.push_back(std::make_shared<SegmentEndMessage>(newSegment->length));

    for (const auto& track : segment.getTracks()) {
        const auto& header = track.getHeader();
        std::string ckid = std::string(header.ckid),
            fccType = std::string(header.fccType);

        ckid.resize(4);
        fccType.resize(4);

        if (ckid == "tetr") {
            loadTempoTrack(track, newSegment->messages);
        } else if (ckid == "cmnd") {
            loadCommandTrack(track, newSegment->messages);
        } else if (*header.ckid == 0 && fccType == "sttr") {
            auto styleTrack = std::static_pointer_cast<StyleTrack>(track.getData());
            for (const auto& style : styleTrack->getStyles()) {
                const std::uint16_t timestamp = style.first;
                const ReferenceList refs = style.second;

                std::string styleFile = refs.getFile();
                auto styleForm = loadStyle(refs.getGuid(), std::string(styleFile.begin(), styleFile.end()));
                assert(styleForm != nullptr);
                std::map<GUID, StylePart> parts;
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

                    newSegment->patterns.push_back(pttn);
                }

                // Load the style's band
                bool firstBand = true;
                for (const auto& band : styleForm->getBands()) {
                    auto message = std::make_shared<BandChangeMessage>(*this, 0, band);
                    newSegment->messages.push_back(message);
                }

                // Load the style's tempo
                newSegment->initialTempo = styleForm->getHeader().dblTempo;
                newSegment->initialSignature = styleForm->getHeader().timeSig;
            }
        } else if (*header.ckid == 0 && fccType == "DMBT") {
            loadBandTrack(track, newSegment->messages, *this);
        } else if (*header.ckid == 0 && fccType == "cord") {
            loadChordTrack(track, newSegment->messages);
        }
    }

    return newSegment;
}

void PlayingContext::playSegment(const SegmentForm& segment, SegmentTiming timing) {
    TRACE("Begin segment play");

    auto newSegment = prepareSegment(segment);

    m_queueMutex.lock();
    if (m_primarySegment == nullptr) {
        m_primarySegment = newSegment;
        enqueueSegment(m_primarySegment);
    } else {
        m_nextSegment = newSegment;
    }
    m_queueMutex.unlock();
}

void PlayingContext::playSegment(std::shared_ptr<SegmentInfo> segment, SegmentTiming timing) {
    TRACE("Begin segment play");

    m_queueMutex.lock();
    if (m_primarySegment == nullptr) {
        m_primarySegment = segment;
        enqueueSegment(m_primarySegment);
    } else {
        m_nextSegment = segment;
    }
    m_queueMutex.unlock();
}

bool PlayingContext::getRandomPattern(const SegmentInfo& segm, std::uint8_t grooveLevel, Pattern* output) const {
    std::vector<int> suitablePatterns;
    for (int i = 0; i < segm.patterns.size(); i++) {
        const auto& pattern = segm.patterns[i];
        if (pattern.header.bGrooveBottom <= grooveLevel &&
            pattern.header.bGrooveTop >= grooveLevel &&
            pattern.header.wEmbellishment == DMUS_EMBELLISHT_NORMAL) {
            suitablePatterns.push_back(i);
        }
    }

    if (suitablePatterns.size() == 0) {
        return false;
    } else if (suitablePatterns.size() == 1) {
        *output = segm.patterns[suitablePatterns[0]];
        return true;
    } else {
        int idx = std::rand();
        *output = segm.patterns[suitablePatterns[idx % suitablePatterns.size()]];
        return true;
    }
}

std::shared_ptr<DirectMusic::DLS::DownloadableSound> PlayingContext::loadInstrumentCollection(const GUID& guid, const std::string& file) {
    std::shared_ptr<DirectMusic::DLS::DownloadableSound> band = nullptr;

    if (m_bands.find(guid) == m_bands.end()) {
        TRACE("Band found in cache");
        std::vector<std::uint8_t> data = m_loader(file);
        band = genObjFromChunkData<DirectMusic::DLS::DownloadableSound>(data);
        m_bands[guid] = band;
    } else {
        TRACE("Loading new band");
        band = m_bands.at(guid);
    }

    return band;
}

std::shared_ptr<StyleForm> PlayingContext::loadStyle(const GUID& guid, const std::string& file) {
    std::shared_ptr<StyleForm> style = nullptr;

    if (m_styles.find(guid) == m_styles.end()) {
        TRACE("Style found in cache");
        std::vector<std::uint8_t> data = m_loader(file);
        style = genObjFromChunkData<StyleForm>(data);
        m_styles[guid] = style;
    } else {
        TRACE("Loading new style");
        style = m_styles.at(guid);
    }

    return style;
}
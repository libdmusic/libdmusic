#include <dmusic/PlayingContext.h>
#include <dmusic/Tracks.h>
#include "MusicMessages.h"

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
                m_messageQueue.push(message);
            }
        } else if (ckid == "cmnd") {
            auto commandTrack = std::static_pointer_cast<CommandTrack>(track.getData());
            for (const auto& command : commandTrack->getCommands()) {
                auto message = std::make_shared<GrooveLevelMessage>(command.mtTime, command.bGrooveLevel, command.bGrooveRange);
                m_messageQueue.push(message);
            }
        } else if (ckid == "" && fccType == "sttr") {
            // TODO: Style track
        } else if (ckid == "" && fccType == "DMBT") {
            auto bandTrack = std::static_pointer_cast<BandTrack>(track.getData());
            for (const auto& band : bandTrack->getBands()) {
                DMUS_IO_BAND_ITEM_HEADER2 header = std::get<0>(band);
                BandForm bandForm = std::get<1>(band);
                
                auto message = std::make_shared<BandChangeMessage>(*this, header.lBandTimePhysical, bandForm);
                m_messageQueue.push(message);
            }
        }
    }
}
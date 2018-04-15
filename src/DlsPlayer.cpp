#if DMUSIC_HAS_SNDFILE && DMUSIC_DLS_PLAYER
#include <dmusic/DlsPlayer.h>
#include <cassert>
#include <exception>
#include <memory>
#include <cmath>
#include <sstream>
#include <sf2cute.hpp>
#include "decode.h"
#define TSF_IMPLEMENTATION
#include "../utils/common/tsf.hxx"
using namespace DirectMusic;
using namespace DirectMusic::DLS;
using namespace sf2cute;

std::unordered_map<DownloadableSound, std::shared_ptr<TinySoundFont>> DlsPlayer::m_soundfonts;

static double dwordTimecentsToSeconds(std::int32_t tc) {
    return exp2((double)tc / (1200.0 * 65536.0));
}

static std::int16_t secondsToWordTimecents(double secs) {
    return (std::int16_t)(1200 * log2(secs));
}

// FIXME: Only "envelope" articulators are supported
static void insertArticulator(const DLS::Articulator& articulator, std::vector<SFModulatorItem>& mods, std::vector<SFGeneratorItem>& gens) {
    for (const auto& connBlock : articulator.getConnectionBlocks()) {
        if (connBlock.usControl == (DLS::ArticulatorControl)0 &&
            connBlock.usSource == DLS::ArticulatorSource::None &&
            connBlock.usTransform == DLS::ArticulatorTransform::None) {
            SFGenerator gen;
            switch (connBlock.usDestination) {
            case DLS::ArticulatorDestination::EG1AttachTime:
                gen = SFGenerator::kAttackVolEnv; break;
            case DLS::ArticulatorDestination::EG1DecayTime:
                gen = SFGenerator::kDecayVolEnv; break;
            case DLS::ArticulatorDestination::EG1ReleaseTime:
                gen = SFGenerator::kReleaseVolEnv; break;
            case DLS::ArticulatorDestination::EG1SustainLevel:
                gen = SFGenerator::kSustainVolEnv; break;

            case DLS::ArticulatorDestination::EG2AttachTime:
                gen = SFGenerator::kAttackModEnv; break;
            case DLS::ArticulatorDestination::EG2DecayTime:
                gen = SFGenerator::kDecayModEnv; break;
            case DLS::ArticulatorDestination::EG2ReleaseTime:
                gen = SFGenerator::kReleaseModEnv; break;
            case DLS::ArticulatorDestination::EG2SustainLevel:
                gen = SFGenerator::kSustainModEnv; break;

            default:
                continue;
            }
            double secs = dwordTimecentsToSeconds(connBlock.lScale);
            std::int16_t newValue = secondsToWordTimecents(secs);
            gens.push_back(SFGeneratorItem(gen, GenAmountType(newValue)));
        }
    }
}

static std::shared_ptr<TinySoundFont> convertCollection(DirectMusic::DLS::DownloadableSound& dls) {
    std::vector<SFSample> samples;
    SoundFont sf2;

    auto& wavePool = dls.getWavePool();

    while(!wavePool.empty()) {
        auto& wav = wavePool[0];
        std::string name = wav.getInfo().getName();
        auto fmt = wav.getWaveformat();

        // DLS lev. 1 only supports PCM16 samples, but
        // we need to load encoded samples as well, so
        // we make libsndfile take care of that
        std::vector<std::int16_t> audioData = decode(wav);

        if (audioData.empty()) {
            throw std::runtime_error("Invalid sample format for " + name);
        }

        auto wavsmpl = wav.getWavesample();
        std::uint32_t startLoop, endLoop;
        std::uint32_t midiNote, fineTune;

        if (wavsmpl.cSampleLoops == 0) {
            // In case the sample doesn't have loops,
            // we just loop the last two samples.
            startLoop = audioData.size() - 2;
            endLoop = audioData.size() - 1;
        } else {
            auto waveLoop = wav.getWavesampleLoops()[0];
            startLoop = waveLoop.ulLoopStart;
            endLoop = waveLoop.ulLoopStart + waveLoop.ulLoopLength;
        }

        // Frees memory which is not needed anymore
        wavePool.erase(wavePool.begin());

        midiNote = wavsmpl.usUnityNote;
        fineTune = wavsmpl.sFineTune;

        samples.push_back(SFSample(name,
            audioData,
            startLoop, endLoop,
            fmt.dwSamplesPerSec,
            midiNote,
            fineTune)); // FIXME: we may need to do some work on this
    }

    std::vector<std::shared_ptr<SFInstrument>> instruments;
    std::vector<std::shared_ptr<SFPreset>> presets;

    for (const auto& instr : dls.getInstruments()) {
        std::vector<SFInstrumentZone> zones;
        for (const auto& reg : instr.getRegions()) {
            auto hdr = reg.getRegionHeader();
            auto wavelink = reg.getWaveLink();
            auto wavesample = reg.getWavesample();
            std::vector<SFGeneratorItem> genItems;
            std::vector<SFModulatorItem> modItems;
            std::shared_ptr<SFSample> sample;
            std::uint16_t keyrangeLow, keyrangeHigh, velrangeLow, velrangeHigh;
            genItems.push_back(SFGeneratorItem(SFGenerator::kAttackVolEnv, GenAmountType(secondsToWordTimecents(0.1))));

            for (const auto& art : instr.getArticulators()) {
                insertArticulator(art, modItems, genItems);
            }

            assert(hdr.RangeKey.usHigh >= hdr.RangeKey.usLow);

            keyrangeLow = hdr.RangeKey.usLow;
            keyrangeHigh = hdr.RangeKey.usHigh;

            if (hdr.RangeVelocity.usHigh - hdr.RangeVelocity.usLow <= 0) {
                velrangeLow = 0;
                velrangeHigh = 127;
            } else {
                velrangeLow = hdr.RangeVelocity.usLow;
                velrangeHigh = hdr.RangeVelocity.usHigh;
            }

            genItems.push_back(SFGeneratorItem(SFGenerator::kKeyRange, RangesType(keyrangeLow, keyrangeHigh)));
            genItems.push_back(SFGeneratorItem(SFGenerator::kVelRange, RangesType(velrangeLow, velrangeHigh)));
            if (wavesample.cSampleLoops == 0) {
                sample = sf2.NewSample(samples[wavelink.ulTableIndex]);
                sample->set_start_loop(sample->data().size() - 2);
                sample->set_end_loop(sample->data().size() - 1);
                genItems.push_back(SFGeneratorItem(SFGenerator::kSampleModes, std::uint16_t(SampleMode::kNoLoop)));
            } else {
                auto loop = reg.getWavesampleLoops()[0];
                sample = sf2.NewSample(samples[wavelink.ulTableIndex]);
                sample->set_start_loop(loop.ulLoopStart);
                sample->set_end_loop(loop.ulLoopStart + loop.ulLoopLength);
                genItems.push_back(SFGeneratorItem(SFGenerator::kSampleModes, std::uint16_t(SampleMode::kLoopContinuously)));
            }
            sample->set_original_key(wavesample.usUnityNote);
            sample->set_correction(wavesample.sFineTune);
            SFInstrumentZone zone(sample, genItems, modItems);
            zones.push_back(zone);
        }
        auto instrument = sf2.NewInstrument(instr.getInfo().getName(), zones);
        instruments.push_back(instrument);
        presets.push_back(sf2.NewPreset(instrument->name(), instr.getMidiProgram(), 0,
            std::vector<SFPresetZone>{
            SFPresetZone(instrument)
        }));
    }

    std::stringstream stream;
    sf2.Write(stream);
    return std::make_shared<TinySoundFont>(stream);
}

static float gainToDecibels(float gain) {
    return 10 * log10(gain);
}

DlsPlayer::DlsPlayer(std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
    DirectMusic::DLS::DownloadableSound& dls,
    const GUID& bandId,
    std::uint32_t sampleRate,
    std::uint32_t channels,
    float volume,
    float pan)
    : InstrumentPlayer(bankLo, bankHi, patch, dls, sampleRate, channels, volume, pan)
    , m_soundfont(nullptr) {
    assert(channels <= 2);

    std::shared_ptr<TinySoundFont> soundfont;
    if (m_soundfonts.find(dls) == m_soundfonts.end()) {
        soundfont = convertCollection(dls);
        TSFOutputMode outputMode = m_channels == 1 ? TSF_MONO : TSF_STEREO_INTERLEAVED;
        soundfont->setOutput(outputMode, sampleRate);

        m_soundfonts[dls] = soundfont;
    } else {
        soundfont = std::make_shared<TinySoundFont>(*m_soundfonts[dls]);
    }

    std::uint32_t bank = (bankHi << 16) + bankLo;

    m_pan = pan < -1 ? -1 : pan > 1 ? 1 : pan;
    m_soundfont = soundfont;

    m_preset = m_soundfont->getPresetIndex(0, patch);
    assert(m_preset >= 0);

    float volFactorRight = sqrt((m_pan + 1) / 2);
    float volFactorLeft = sqrt((-m_pan + 1) / 2);

    m_soundfont->setPresetPanning(m_preset, volFactorLeft, volFactorRight);
    m_soundfont->setPresetGain(m_preset, gainToDecibels(m_volume));
}

std::uint32_t DlsPlayer::renderBlock(std::int16_t *buffer, std::uint32_t count, bool mix) noexcept {
    m_soundfont->renderSamples(buffer, count / m_channels, mix);
    return count;
}

/// Instructs the synthesizer to start playing a note
void DlsPlayer::noteOn(std::uint8_t note, std::uint8_t velocity) {
    m_soundfont->noteOn(m_preset, note, velocity / 255.0f);
}

/// Instructs the synthesizer to stop playing a note
void DlsPlayer::noteOff(std::uint8_t note, std::uint8_t velocity) {
    m_soundfont->noteOff(m_preset, note);
}

void DlsPlayer::allNotesOff() {
    m_soundfont->allNotesOff(m_preset);
}

/// Sends a "channel pressure" message
void DlsPlayer::channelPressure(std::uint8_t val) {}

/// Sends a "polyphonic aftertouch" message to a note
void DlsPlayer::polyAftertouch(std::uint8_t note, std::uint8_t val) {}

/// Sends a "control change" message
void DlsPlayer::controlChange(DirectMusic::Midi::Control control, float val) {
    if (control == DirectMusic::Midi::Control::ChannelVolume || control == DirectMusic::Midi::Control::ExpressionCtl) {
        m_volume = val;
        m_soundfont->setPresetGain(m_preset, gainToDecibels(m_volume * m_volume * m_volume * m_volume));
    }
}

/// Sends a "program change" (aka "patch change") message
void DlsPlayer::programChange(std::uint8_t program) {}

/// Sends a "pitch bend" message
void DlsPlayer::pitchBend(std::int16_t val) {}

PlayerFactory DlsPlayer::createFactory() {
    return [](std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
        const GUID& bandGuid, DownloadableSound& dls, std::uint32_t sampleRate, std::uint32_t chans, float vol, float pan) -> std::shared_ptr<InstrumentPlayer> {

        return std::shared_ptr<DlsPlayer>{
            new DlsPlayer(bankLo, bankHi, patch, dls, bandGuid, sampleRate, chans, vol, pan)
        };
    };
}

GMPlayerFactory DlsPlayer::createGMFactory(DownloadableSound& dls) {
    return [&](std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
        std::uint32_t sampleRate, std::uint32_t chans, float vol, float pan) -> std::shared_ptr<InstrumentPlayer> {

        return std::shared_ptr<DlsPlayer>{
            new DlsPlayer(bankLo, bankHi, patch, dls, GUID(), sampleRate, chans, vol, pan)
        };
    };
}

#endif
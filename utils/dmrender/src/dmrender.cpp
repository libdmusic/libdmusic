#include <iostream>
#include <queue>
#include <locale>
#include <dmusic/PlayingContext.h>
#include <dmusic/InstrumentPlayer.h>
#include <dmusic/Tracks.h>
#include <dmusic/dls/DownloadableSound.h>

using namespace DirectMusic;
using namespace DirectMusic::DLS;

static std::queue<std::wstring> styles;

class MyInstrumentPlayer : public InstrumentPlayer {
public:
    MyInstrumentPlayer(std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
        const DownloadableSound& dls,
        std::uint32_t sampleRate,
        std::uint32_t channels) : InstrumentPlayer(bankLo, bankHi, patch, dls, sampleRate, channels) {

        std::uint32_t bank = (bankHi << 16) + bankLo;

        for (const auto& instr : dls.getInstruments()) {
            if (instr.getMidiBank() == bank && instr.getMidiProgram() == patch) {
                std::cout << "Instrument loaded: " << instr.getInfo().getName() << " from " << dls.getInfo().getName() << "\n";
            }
        }
    }
    virtual std::uint32_t renderBlock(std::int16_t *buffer, std::uint32_t count) noexcept { return count; }

    /// Instructs the synthesizer to start playing a note
    virtual void noteOn(std::uint8_t note, std::uint8_t velocity) {}

    /// Instructs the synthesizer to stop playing a note
    virtual void noteOff(std::uint8_t note, std::uint8_t velocity) {}

    /// Sends a "channel pressure" message
    virtual void channelPressure(std::uint8_t val) {}

    /// Sends a "polyphonic aftertouch" message to a note
    virtual void polyAftertouch(std::uint8_t note, std::uint8_t val) {}

    /// Sends a "control change" message
    virtual void controlChange(DirectMusic::Midi::Control control, std::int32_t val) {}

    /// Sends a "program change" (aka "patch change") message
    virtual void programChange(std::uint8_t program) {}

    /// Sends a "pitch bend" message
    virtual void pitchBend(std::int16_t val) {}
};

static std::string getChordName(std::uint32_t chord) {
    std::string table[12] = {
        "C", "C#", "D", "D#",
        "E", "F", "F#", "G",
        "G#", "A", "A#", "B"
    };
    std::string noteName = table[chord % 12];
    int octave = (chord - (chord % 12)) / 12;
    return noteName + std::to_string(octave);
}

static void printBand(const BandForm& band) {
    for (const auto& instr : band.getInstruments()) {
        auto ref = instr.getReference();
        if (ref != nullptr) {
            std::wcout << ref->getName() << " (" << ref->getFile() << ")" << ".\n";
        }
        auto header = instr.getHeader();
        // I don't know why the first bytes is skipped...
        std::uint8_t bankHi = (header.dwPatch & 0x00FF0000) >> 0x10;
        std::uint8_t bankLo = (header.dwPatch & 0x0000FF00) >> 0x8;
        std::uint8_t patch = (header.dwPatch & 0x000000FF);
        std::cout << "- Instrument - Channel: " << header.dwPChannel << ". Bank Hi: " << (std::uint32_t)bankHi << ", Bank Lo: " << (std::uint32_t)bankLo << ", Patch: " << (std::uint32_t)patch << "\n";
    }
}

template<typename T>
static void printTrack(const T& track) {
    std::cout << "Not implemented.\n";
}

template<>
void printTrack<std::shared_ptr<ChordTrack>>(const std::shared_ptr<ChordTrack>& track) {
    std::uint32_t header = track->getHeader();
    std::cout << getChordName((header & 0xFF000000) >> 24) << "\n";
    std::wcout << std::wstring((const wchar_t *)track->getChord().wszName) << "\n";
    for (const auto& subchord : track->getSubchords()) {
        std::cout << getChordName(subchord.bChordRoot) << "\n";
    }
}

template<>
void printTrack<std::shared_ptr<StyleTrack>>(const std::shared_ptr<StyleTrack>& track) {
    for (const StyleReference& ref : track->getStyles()) {
        std::uint16_t timestamp = std::get<0>(ref);
        ReferenceList refList = std::get<1>(ref);
        std::wcout << refList.getName() << " (" << refList.getFile() << ")" << " at " << timestamp << "\n";
        styles.push(refList.getFile());
    }
}

template<>
void printTrack<std::shared_ptr<BandTrack>>(const std::shared_ptr<BandTrack>& track) {
    std::cout << "Auto download: " << (track->getHeader().bAutoDownload ? "true" : "false") << "\n";
    for (const auto& band : track->getBands()) {
        DMUS_IO_BAND_ITEM_HEADER2 header = std::get<0>(band);
        BandForm bandForm = std::get<1>(band);
        printBand(bandForm);
    }
}

template<>
void printTrack<std::shared_ptr<TempoTrack>>(const std::shared_ptr<TempoTrack>& track) {
    for (const auto& item : track->getItems()) {
        std::cout << "Tempo: " << item.dblTempo << " at " << item.lTime << "\n";
    }
}

template<>
void printTrack<std::shared_ptr<CommandTrack>>(const std::shared_ptr<CommandTrack>& track) {
    for (const auto& command : track->getCommands()) {
        switch (command.bCommand) {
        case DMUS_COMMANDT_TYPES::DMUS_COMMANDT_BREAK:
            std::cout << "Break command ";
            break;
        case DMUS_COMMANDT_TYPES::DMUS_COMMANDT_END:
            std::cout << "End command ";
            break;
        case DMUS_COMMANDT_TYPES::DMUS_COMMANDT_ENDANDINTRO:
            std::cout << "End and intro command ";
            break;
        case DMUS_COMMANDT_TYPES::DMUS_COMMANDT_FILL:
            std::cout << "Fill command ";
            break;
        case DMUS_COMMANDT_TYPES::DMUS_COMMANDT_GROOVE:
            std::cout << "Groove command ";
            break;
        case DMUS_COMMANDT_TYPES::DMUS_COMMANDT_INTRO:
            std::cout << "Intro command ";
            break;
        default:
            std::cout << "Invalid command ";
            break;
        }
        std::cout << "at " << command.wMeasure << ":" << (std::uint32_t)command.bBeat << "\n";
    }
}

template<>
void printTrack<TrackForm>(const TrackForm& track) {
    const auto& header = track.getHeader();
    std::string ckid = std::string(header.ckid),
        fccType = std::string(header.fccType);
    if (ckid == "") {
        if (fccType == "cord") {
            std::cout << "- Chord track\n";
            printTrack(std::static_pointer_cast<ChordTrack>(track.getData()));
        } else if (fccType == "pftr") {
            std::cout << "- Chordmap track\n";
            printTrack(std::static_pointer_cast<ChordmapTrack>(track.getData()));
        } else if (fccType == "lyrt") {
            std::cout << "- Lyrics track\n";
            printTrack(std::static_pointer_cast<LyricsTrack>(track.getData()));
        } else if (fccType == "MARK") {
            std::cout << "- Marker track\n";
            printTrack(std::static_pointer_cast<MarkerTrack>(track.getData()));
        } else if (fccType == "prmt") {
            std::cout << "- Parameter control track\n";
            printTrack(std::static_pointer_cast<ParameterControlTrack>(track.getData()));
        } else if (fccType == "scrt") {
            std::cout << "- Script track\n";
            printTrack(std::static_pointer_cast<ScriptTrack>(track.getData()));
        } else if (fccType == "wavt") {
            std::cout << "- Wave track\n";
            printTrack(std::static_pointer_cast<WaveTrack>(track.getData()));
        } else if (fccType == "segt") {
            std::cout << "- Segment trigger track\n";
            printTrack(std::static_pointer_cast<SegmentTriggerTrack>(track.getData()));
        } else if (fccType == "sttr") {
            std::cout << "- Style track\n";
            printTrack(std::static_pointer_cast<StyleTrack>(track.getData()));
        } else if (fccType == "TIMS") {
            std::cout << "- Time signature track\n";
            printTrack(std::static_pointer_cast<TimeSignatureTrack>(track.getData()));
        } else if (fccType == "DMBT") {
            std::cout << "- Band track\n";
            printTrack(std::static_pointer_cast<BandTrack>(track.getData()));
        } else if (fccType == "DMPT") {
            std::cout << "- Pattern track\n";
            printTrack(std::static_pointer_cast<PatternTrack>(track.getData()));
        }
    } else if (ckid == "cmnd") {
        std::cout << "- Command track\n";
        printTrack(std::static_pointer_cast<CommandTrack>(track.getData()));
    } else if (ckid == "mute") {
        std::cout << "- Mute track\n";
        printTrack(std::static_pointer_cast<MuteTrack>(track.getData()));
    } else if (ckid == "sgnp") {
        std::cout << "- Signpost track\n";
        printTrack(std::static_pointer_cast<SignpostTrack>(track.getData()));
    } else if (ckid == "syex") {
        std::cout << "- Sysex track\n";
        printTrack(std::static_pointer_cast<SysexTrack>(track.getData()));
    } else if (ckid == "tetr") {
        std::cout << "- Tempo track\n";
        printTrack(std::static_pointer_cast<TempoTrack>(track.getData()));
    }
}

void printStyle(const std::shared_ptr<StyleForm> style) {
    std::wcout << "Style name: " << style->getInfo().getName() << "\n";
    auto header = style->getHeader();
    std::cout << "Tempo: " << header.dblTempo << " BPM " << (std::uint32_t)header.timeSig.bBeatsPerMeasure << "/" << (std::uint32_t)header.timeSig.bBeat << "\n";
    const auto& parts = style->getParts();
    std::cout << parts.size() << " parts:\n";
    for (const auto& part : parts) {
        std::wcout << part.getInfo().getName() << ": "
            << part.getCurves().size() << " curves, "
            << part.getMarkers().size() << " markers, "
            << part.getNotes().size() << " notes, "
            << part.getResolutions().size() << " resolutions, "
            << part.getAnticipations().size() << " anticipations.\n";
    }

    const auto& patterns = style->getPatterns();
    std::cout << "\n" << patterns.size() << " patterns:\n";
    for (const auto& pattern : patterns) {
        const auto& patternParts = pattern.getParts();
        std::wcout << pattern.getInfo().getName() << ": "
            << pattern.getRhythms().size() << " rhythms, "
            << patternParts.size() << " parts.\n";
        if (pattern.getMotifSettings() != nullptr) {
            const auto motifSettings = pattern.getMotifSettings();
            std::cout << "To be looped " << motifSettings->dwRepeats << " times, from " << motifSettings->mtLoopStart << " to " << motifSettings->mtLoopEnd << ".\n";
        }

        for (const auto& part : patternParts) {
            std::wcout << "    " << part.getInfo().getName() << ": "
                << part.getCurves().size() << " curves, "
                << part.getMarkers().size() << " markers, "
                << part.getNotes().size() << " notes, "
                << part.getResolutions().size() << " resolutions, "
                << part.getAnticipations().size() << " anticipations.\n";
        }
    }

    for (const auto& band : style->getBands()) {
        printBand(band);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: dmrender [inputfile]\n";
        return 1;
    }
    PlayingContext ctx(44100, 1, [](std::uint8_t bankLo, std::uint8_t bankHi, std::uint8_t patch,
        const DownloadableSound& dls, std::uint32_t sampleRate, std::uint32_t channels) {
        return std::static_pointer_cast<InstrumentPlayer>(std::make_shared<MyInstrumentPlayer>(bankLo, bankHi, patch, dls, sampleRate, channels));
    });
    std::cout << "Loading segment...";
    auto segment = ctx.loadSegment(argv[1]);
    std::cout << " done.\n" << segment->getTracks().size() << " tracks found:\n";
    for (const auto& track : segment->getTracks()) {
        printTrack(track);
    }
    std::cout << "Segment must be repeated " << segment->getHeader().dwRepeats << " times.\n";
    std::cout << "Segment length: " << segment->getHeader().mtLength << " pulses.\n";
    std::cout << "Loop start: " << segment->getHeader().mtLoopStart << ", loop end: " << segment->getHeader().mtLoopEnd << "\n";

    std::cout << "\n---\n\nLoading styles...\n";

    while (!styles.empty()) {
        std::wstring styleFile = styles.front();
        styles.pop();
        auto style = ctx.loadStyle(std::string(styleFile.begin(), styleFile.end()));
        printStyle(style);
    }

    ctx.playSegment(*segment);

    int a;
    std::cin >> a;
    return 0;
}

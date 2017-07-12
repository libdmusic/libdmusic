#include <iostream>
#include <dmusic/PlayingContext.h>
#include <dmusic/InstrumentPlayer.h>
#include <dmusic/Tracks.h>
#include <dmusic/dls/DownloadableSound.h>

using namespace DirectMusic;
using namespace DirectMusic::DLS;

class MyInstrumentPlayer : public InstrumentPlayer {
public:
    MyInstrumentPlayer(const Instrument& instr, std::uint32_t sampleRate, std::uint32_t channels) : InstrumentPlayer(instr, sampleRate, channels) {}
    virtual std::uint32_t renderBlock(std::int16_t *buffer, std::uint32_t count) noexcept { return count; }

    /// Instructs the synthesizer to start playing a note
    virtual bool noteOn(std::uint8_t note, std::uint8_t velocity) {}

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
        std::wcout << refList.getName() << " (" << refList.getFile() << ")" << "\n";
    }
}

template<>
void printTrack<std::shared_ptr<BandTrack>>(const std::shared_ptr<BandTrack>& track) {
    std::cout << "Auto download: " << (track->getHeader().bAutoDownload ? "true" : "false") << "\n";
    for (const auto& band : track->getBands()) {
        DMUS_IO_BAND_ITEM_HEADER2 header = std::get<0>(band);
        BandForm bandForm = std::get<1>(band);
        for (const auto& instr : bandForm.getInstruments()) {
            auto ref = instr.getReference();
            std::wcout << ref->getName() << " (" << ref->getFile() << ")" << "\n";
            auto header = instr.getHeader();
            std::cout << "Channel: " << header.dwPChannel << "\n";
        }
    }
}

template<>
void printTrack<std::shared_ptr<TempoTrack>>(const std::shared_ptr<TempoTrack>& track) {
    for (const auto& item : track->getItems()) {
        std::cout << "Tempo: " << item.dblTempo << " at " << item.lTime << "\n";
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
    } else if(ckid == "cmnd") {
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

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Usage: dmrender [inputfile]\n";
        return 1;
    }
    PlayingContext<MyInstrumentPlayer> ctx(44100, 1);
    std::cout << "Loading segment...";
    auto segment = ctx.loadSegment(argv[1]);
    std::cout << " done.\n" << segment->getTracks().size() << " tracks found:\n";
    for (const auto& track : segment->getTracks()) {
        printTrack(track);
    }
    return 0;
}

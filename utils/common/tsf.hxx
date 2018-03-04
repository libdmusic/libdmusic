#pragma once

#include <sstream>
#include "tsf.h"

class TinySoundFont {
private:
    tsf* m_soundfont;

public:
    TinySoundFont(std::stringstream& stream) {
        std::string buffer = stream.str();
        m_soundfont = tsf_load_memory(buffer.c_str(), buffer.size());
    }

    TinySoundFont(const TinySoundFont& soundfont) {
        m_soundfont = tsf_copy(soundfont.m_soundfont);
    }

    ~TinySoundFont() {
        tsf_close(m_soundfont);
    }

    int getPresetIndex(int bank, int presetNumber) const {
        return tsf_get_presetindex(m_soundfont, bank, presetNumber);
    }

    int getPresetCount() const {
        return tsf_get_presetcount(m_soundfont);
    }

    void setOutput(enum TSFOutputMode outputmode, int samplerate, float global_gain_db = 0) {
        tsf_set_output(m_soundfont, outputmode, samplerate, global_gain_db);
    }

    void setPanning(float pan_factor_left, float pan_factor_right) {
        tsf_set_panning(m_soundfont, pan_factor_left, pan_factor_right);
    }

    void setPresetPanning(int preset, float left, float right) {
        tsf_set_preset_panning(m_soundfont, preset, left, right);
    }

    void setPresetGain(int preset, float gain) {
        tsf_set_preset_gain(m_soundfont, preset, gain);
    }

    void noteOn(int preset_index, int key, float vel) {
        tsf_note_on(m_soundfont, preset_index, key, vel);
    }

    void noteOff(int preset_index, int key) {
        tsf_note_off(m_soundfont, preset_index, key);
    }

    void allNotesOff(int preset) {
        tsf_all_notes_off(m_soundfont, preset);
    }

    void renderSamples(short* buffer, int samples, bool mixing) {
        tsf_render_short(m_soundfont, buffer, samples, mixing ? 1 : 0);
    }

    void renderSamples(float* buffer, int samples, bool mixing) {
        tsf_render_float(m_soundfont, buffer, samples, mixing ? 1 : 0);
    }
};
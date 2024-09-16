/*
 * Copyright (c) 2024. Jonathan Bisson
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef SYNTH_H
#define SYNTH_H

#include <Arduino.h>
#include <MIDI.h>
#include <I2S.h>


#include "device.h"
#include "display.h"
#include "storage.h"
#include "pra32-u.h"


enum class SynthModeState {
  NORMAL,
};



namespace SynthParameters {

struct Parameters {
  uint8_t cc;
  const char* name;
  const char* short_name;
};

struct Page {
  uint8_t paramCount;
  const Parameters* params;
  const char* name;
};

struct Interface {
  uint8_t pageCount;
  const Page* pages;
};

constexpr Parameters MODULATION = { 1, "Modulation", "MOD" }; // p1
constexpr Parameters BREATH = { 2, "Breath", "BRH" };
constexpr Parameters OSC1_WAVE = { 102, "Osc 1 Wave", "O1W" }; // p1
constexpr Parameters OSC1_SHAPE = { 19, "Osc 1 Shape", "O1S" }; // p1
constexpr Parameters OSC1_MORPH = { 20, "Osc 1 Morph", "O1M" }; // p1

constexpr Parameters MIXER_NOISE_SUB = { 23, "Mixer noise/sub", "MXN" };

constexpr Parameters OSC2_WAVE = { 104, "Osc 2 Wave", "O2W" }; //p1
constexpr Parameters OSC2_COARSE = { 85, "Osc 2 Coarse", "O2C" }; // p1  // Will need high res
constexpr Parameters OSC2_PITCH = { 76, "Osc 2 Pitch", "O2P" }; // p1   // Will need high res
constexpr Parameters MIXER_OSC_MIX = { 21, "Mixer Osc Mix", "MXO" };

constexpr Parameters FILTER_CUTOFF = { 74, "Filter cutoff", "FCO" };
constexpr Parameters FILTER_RESONANCE = { 71, "Filter resonance", "FRE" };
constexpr Parameters FILTER_EG_AMOUNT = { 24, "Filter EG amount", "FEG" };
constexpr Parameters FILTER_KEY_TRACK = { 26, "Filter Key track", "FKT" };

constexpr Parameters EG_ATTACK = { 73, "EG Attack", "EGA" };
constexpr Parameters EG_DECAY = { 75, "EG Decay", "EGD" };
constexpr Parameters EG_SUSTAIN = { 30, "EG Sustain", "EGS" };
constexpr Parameters EG_RELEASE = { 72, "EG Release", "EGR" };

constexpr Parameters EG_OSC_AMT = { 91, "EG Osc Amt", "EOA" };
constexpr Parameters EG_OSC_DST = { 89, "EG Osc Dst", "EOD" };

constexpr Parameters VOICE_MODE = { 14, "Voice Mode", "VMO" };
constexpr Parameters PORTAMENTO = { 5, "Portamento", "POR" };

constexpr Parameters LFO_WAVE = { 12, "LFO Wave", "LFW" };
constexpr Parameters LFO_RATE = { 3, "LFO Rate", "LFR" };
constexpr Parameters LFO_DEPTH = { 17, "LFO Depth", "LFD" };
constexpr Parameters LFO_FADE_TIME = { 56, "LFO Fade Time", "LFF" };

constexpr Parameters LFO_OSC_AMT = { 13, "LFO Osc Amt", "LOA" };
constexpr Parameters LFO_OSC_DST = { 103, "LFO Osc Dst", "LOD" };
constexpr Parameters LFO_FILTER_AMT = { 25, "LFO Filter Amt", "LFA" };

constexpr Parameters AMP_GAIN = { 15, "Amp Gain", "AG" };

constexpr Parameters AMP_ATTACK = { 52, "Amp Attack", "AMA" };
constexpr Parameters AMP_DECAY = { 53, "Amp Decay", "AMD" };
constexpr Parameters AMP_SUSTAIN = { 54, "Amp Sustain", "AMS" };
constexpr Parameters AMP_RELEASE = { 55, "Amp Release", "AMR" };

constexpr Parameters FILTER_MODE = { 78, "Filter Mode", "FMO" };
constexpr Parameters EG_AMP_MOD = { 28, "EG Amp Mod", "EAM" };
constexpr Parameters RELEASE_DECAY = { 29, "Release = Decay", "RDC" };

constexpr Parameters PITCH_BEND_RANGE = { 57, "Pitch Bend Range", "PBR" };

constexpr Parameters BREATH_FILTER_AMT = { 60, "Breath Filter Amt", "BFA" };
constexpr Parameters BREATH_AMP_MOD = { 61, "Breath Amp Mod", "BAM" };
constexpr Parameters EG_VELOCITY_SENSITIVITY = { 62, "EG Velocity Sensitivity", "EVS" };
constexpr Parameters AMP_VELOCITY_SENSITIVITY = { 63, "Amp Velocity Sensitivity", "AVS" };

constexpr Parameters CHORUS_MIX = { 27, "Chorus Mix", "CMX" };
constexpr Parameters CHORUS_RATE = { 58, "Chorus Rate", "CRT" };
constexpr Parameters CHORUS_DEPTH = { 59, "Chorus Depth", "CDP" };

constexpr Parameters DELAY_FEEDBACK = { 92, "Delay Feedback", "DFB" };
constexpr Parameters DELAY_TIME = { 90, "Delay Time", "DTM" };
constexpr Parameters DELAY_MODE = { 35, "Delay Mode", "DMD" };

constexpr Parameters PROGRAM_NUMBER_WRITE = { 87, "Program Number to Write to", "PNW" };
constexpr Parameters WRITE_PARAMETERS_PROGRAM = { 106, "Write Parameters to Program", "WPP" };

constexpr Parameters page1_params[] = { OSC1_WAVE, OSC1_SHAPE, OSC1_MORPH, MODULATION, OSC2_WAVE, OSC2_COARSE, OSC2_PITCH };
constexpr Page Page1 = { 7, page1_params, "Oscillators" };
constexpr Parameters page2_params[] = { }
}


class SynthMode {
public:
  SynthMode(Device& device);
  void begin();
  void setup_core1();
  void loop1();
  void loop();
  void onKnobChange(uint8_t id, uint8_t value);
  void onTouchPadChange(uint8_t id, uint8_t value);
  void onButtonChange(uint8_t id, uint8_t value);
  void update();
  void handleNoteOn(byte channel, byte pitch, byte velocity);
  void handleNoteOff(byte channel, byte pitch, byte velocity);
  void handleControlChange(byte channel, byte number, byte value);
  void handleProgramChange(byte channel, byte number);
  void handlePitchBend(byte channel, int bend);
  void handleClock();
  void handleStart();
  void handleStop();
  void handleKnob(uint8_t knob, uint8_t value);
  void handleTouch(uint8_t pad, uint8_t value);
  void updateAll();

private:
  void display_normal_mode();
  volatile bool updated = false;        // can only be written to by core0
  volatile bool clear_updated = false;  // can only be written to by core1
  Device& device;
  Storage storage;
  SynthModeState current_state = SynthModeState::NORMAL;
  uint8_t last_forced_update = 0;
  bool ready = false;
};


#endif

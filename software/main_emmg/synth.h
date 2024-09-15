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

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

#ifndef DEVICESTATE_H
#define DEVICESTATE_H

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include "config.h"
#include "debouncer.h"
#include "debug.h"
#include "display.h"
#include "piomidi.h"


class Device {
public:
  Device();

  void begin();
  void poll();
  bool isUpdated();

  int getKnobValue(uint8_t index) const;
  bool getButtonValue() const;
  bool getTouchPadValue(uint8_t index) const;
  void setKnobCallback(void (*callback)(uint8_t, uint8_t));
  void setTouchPadCallback(void (*callback)(uint8_t, uint8_t));
  void setButtonCallback(void (*callback)(uint8_t, uint8_t));
  void sendControlChange(uint8_t, uint8_t, uint8_t);
  void sendNoteOn(uint8_t, uint8_t, uint8_t);
  void sendNoteOff(uint8_t, uint8_t, uint8_t);
  void midiPanic();

  void setHandleNoteOn(void (*)(byte channel, byte pitch, byte velocity));
  void setHandleNoteOff(void (*)(byte channel, byte pitch, byte velocity));
  
  Display display;

  midi::MidiInterface<midi::SerialMIDI<Adafruit_USBD_MIDI> >* MIDI = nullptr;
private:
  int knobValues[8];
  bool buttonState;
  Debouncer debounced_button;
  bool touchPadStates[12];
  void (*knobCallback)(uint8_t, uint8_t);
  void (*touchPadCallback)(uint8_t, uint8_t);
  void (*buttonCallback)(uint8_t, uint8_t);

  void triggerKnobCallback(uint8_t index, uint8_t value);
  void triggerTouchPadCallback(uint8_t index, uint8_t value);
  void triggerButtonCallback(uint8_t index, uint8_t value);
};

#endif  // DEVICESTATE_H

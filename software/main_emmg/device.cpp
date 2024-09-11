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

#include "device.h"
#include "display.h"
#include "knobs.h"
#include "touch.h"

Knobs knobs;
MultiTouch touch;


MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI_IF_1);  // Unfortunately the PIO version doesn't work

Device::Device()
  : debounced_button(BUTTON_PIN, 50) {
  for (int i = 0; i < 8; i++) {
    knobValues[i] = 0;
  }
  for (int i = 0; i < 12; i++) {
    touchPadStates[i] = false;
  }

  buttonState = false;
}

void Device::begin() {
  display.begin();
  SERIAL_PRINTLN("Testing display");
  display.test();
  SERIAL_PRINTLN("Calibrate touchpads");
  touch.calibrateAll();
  SERIAL_PRINTLN("Starting the knobs");
  knobs.begin();
  SERIAL_PRINTLN("Setup MIDI");

  MIDI_IF_1.begin(MIDI_CHANNEL_OMNI);

  //usb_midi.setStringDescriptor("EMMG MIDI 2024");



  //while (!TinyUSBDevice.mounted()) {
  // delay(1);
  // }

  debounced_button.init();
  SERIAL_PRINTLN("Device ready");
}

void __not_in_flash_func(Device::poll)() {
#ifdef TINYUSB_NEED_POLLING_TASK
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.task();
  }
#endif
  MIDI->read();
  MIDI_IF_1.read();
  knobs.tick();
  touch.tick();
  debounced_button.tick();

  const int* values = knobs.getValues();
  for (uint8_t i = 0; i < knobs.getSize(); i++) {
    uint8_t value = values[i];
    if (knobValues[i] != value) {
      knobValues[i] = value;
      triggerKnobCallback(i, value);
    }
  }

  const bool* touches = touch.getTouches();
  for (uint8_t i = 0; i < 12; i++) {
    uint8_t value = touches[i];
    if (touchPadStates[i] != value) {
      touchPadStates[i] = value;
      triggerTouchPadCallback(i, value);
    }
  }

  bool value = debounced_button.isPressed();
  if (value != buttonState) {
    buttonState = value;
    triggerButtonCallback(0, value);
  }
}

int Device::getKnobValue(uint8_t index) const {
  if (index < 8) {
    return knobValues[index];
  }
  return -1;  // Error value
}

bool Device::getButtonValue() const {
  return buttonState;
}

bool Device::getTouchPadValue(uint8_t index) const {
  if (index < 12) {
    return touchPadStates[index];
  }
  return false;  // Error value
}


void Device::setKnobCallback(void (*callback)(uint8_t, uint8_t)) {
  knobCallback = callback;
}

void Device::setTouchPadCallback(void (*callback)(uint8_t, uint8_t)) {
  touchPadCallback = callback;
}

void Device::setButtonCallback(void (*callback)(uint8_t, uint8_t)) {
  buttonCallback = callback;
}

void Device::triggerKnobCallback(uint8_t index, uint8_t value) {
  if (knobCallback) {
    knobCallback(index, value);
  }
}

void Device::triggerTouchPadCallback(uint8_t index, uint8_t value) {
  if (touchPadCallback) {
    touchPadCallback(index, value);
  }
}

void Device::triggerButtonCallback(uint8_t index, uint8_t value) {
  if (buttonCallback) {
    buttonCallback(index, value);
  }
}

void Device::sendControlChange(uint8_t index, uint8_t value, uint8_t channel) {
  if (TinyUSBDevice.mounted()) {
    MIDI->sendControlChange(index, value, channel);
  }
  MIDI_IF_1.sendControlChange(index, value, channel);
}

void Device::sendNoteOn(uint8_t index, uint8_t value, uint8_t channel) {
  if (TinyUSBDevice.mounted()) {
    MIDI->sendNoteOn(index, value, channel);
  }
  MIDI_IF_1.sendNoteOn(index, value, channel);
}

void Device::sendNoteOff(uint8_t index, uint8_t value, uint8_t channel) {
  if (TinyUSBDevice.mounted()) {
    MIDI->sendNoteOff(index, value, channel);
  }
  MIDI_IF_1.sendNoteOn(index, value, channel);
}

void Device::midiPanic() {
  for (int i = 0; i < 16; i++) {
    if (TinyUSBDevice.mounted()) {
      MIDI->sendControlChange(123, 0, i);
    }
    MIDI_IF_1.sendControlChange(123, 0, i);
  }
}

void Device::setHandleNoteOn(void (*f)(byte channel, byte pitch, byte velocity)) {
  if (TinyUSBDevice.mounted()) {
    MIDI->setHandleNoteOn(f);
  }
  MIDI_IF_1.setHandleNoteOn(f);
}

void Device::setHandleNoteOff(void (*f)(byte channel, byte pitch, byte velocity)) {
  if (TinyUSBDevice.mounted()) {
    MIDI->setHandleNoteOff(f);
  }
  MIDI_IF_1.setHandleNoteOff(f);
}
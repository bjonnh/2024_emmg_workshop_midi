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

#include <Arduino.h>
#include "synth.h"
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/TomThumb.h>
#include "config.h"

SynthMode* SynthModeInstance = nullptr;

#include "pra32-u/pra32-u-common.h"
#include "pra32-u/pra32-u-synth.h"

// I want that in SynthMode
PRA32_U_Synth g_synth;

// I want that in Device
I2S g_i2s_output(OUTPUT);



SynthMode::SynthMode(Device& device)
  : device(device) {
  SynthModeInstance = this;
}


void SynthMode::begin() {
  g_i2s_output.setSysClk(SAMPLING_RATE);
  g_i2s_output.setFrequency(SAMPLING_RATE);
  g_i2s_output.setDATA(PRA32_U_I2S_DATA_PIN);
  g_i2s_output.setBCLK(PRA32_U_I2S_BCLK_PIN);
  if (PRA32_U_I2S_SWAP_BCLK_AND_LRCLK_PINS) {
    g_i2s_output.swapClocks();
  }
  g_i2s_output.setBitsPerSample(16);
  g_i2s_output.setBuffers(PRA32_U_I2S_BUFFERS, PRA32_U_I2S_BUFFER_WORDS);
  g_i2s_output.begin();

  //#if defined(PRA32_U_USE_USB_MIDI)
  //  TinyUSB_Device_Init(0);
  //  USBDevice.setManufacturerDescriptor("ISGK Instruments");
  // USBDevice.setProductDescriptor("Digital Synth PRA32-U");
  //  USB_MIDI.setHandleNoteOn(handleNoteOn);
  //  USB_MIDI.setHandleNoteOff(handleNoteOff);
  //  USB_MIDI.setHandleControlChange(handleControlChange);
  //  USB_MIDI.setHandleProgramChange(handleProgramChange);
  //  USB_MIDI.setHandlePitchBend(handlePitchBend);
  //  USB_MIDI.setHandleClock(handleClock);
  //  USB_MIDI.setHandleStart(handleStart);
  //  USB_MIDI.setHandleStop(handleStop);
  //  USB_MIDI.begin(MIDI_CHANNEL_OMNI);
  //  USB_MIDI.turnThruOff();
  //#endif  // defined(PRA32_U_USE_USB_MIDI)

  g_synth.initialize();

  delay(100);

  device.setKnobCallback([](uint8_t id, uint8_t value) {
    SynthModeInstance->handleKnob(id, value);
  });
  device.setTouchPadCallback([](uint8_t id, uint8_t value) {
    SynthModeInstance->handleTouch(id, value);
  });
  device.setButtonCallback([](uint8_t id, uint8_t value) {
  });
  device.setHandleNoteOn([](byte channel, byte pitch, byte velocity) {
    SynthModeInstance->handleNoteOn(channel, pitch, velocity);
  });
  device.setHandleNoteOff([](byte channel, byte pitch, byte velocity) {
    SynthModeInstance->handleNoteOff(channel, pitch, velocity);
  });

  //  USB_MIDI.setHandleControlChange(handleControlChange);
  //  USB_MIDI.setHandleProgramChange(handleProgramChange);
  //  USB_MIDI.setHandlePitchBend(handlePitchBend);
  //  USB_MIDI.setHandleClock(handleClock);
  //  USB_MIDI.setHandleStart(handleStart);
  //  USB_MIDI.setHandleStop(handleStop);
}

void SynthMode::setup_core1() {
}

void __not_in_flash_func(SynthMode::loop1)() {
  boolean processed = g_synth.secondary_core_process();
  if (processed) {
    static uint32_t s_loop_counter = 0;
    s_loop_counter++;
    if (s_loop_counter >= 16 * 400) {
      s_loop_counter = 0;
    }
  }
}

void __not_in_flash_func(SynthMode::loop)() {
  device.poll();
  int16_t left_buffer[PRA32_U_I2S_BUFFER_WORDS];
  int16_t right_buffer[PRA32_U_I2S_BUFFER_WORDS];
  for (uint32_t i = 0; i < PRA32_U_I2S_BUFFER_WORDS; i++) {
    left_buffer[i] = g_synth.process(right_buffer[i]);
  }

  for (uint32_t i = 0; i < PRA32_U_I2S_BUFFER_WORDS; i++) {
    if (PRA32_U_I2S_SWAP_LEFT_AND_RIGHT) {
      g_i2s_output.write16(right_buffer[i], left_buffer[i]);
    } else {
      g_i2s_output.write16(left_buffer[i], right_buffer[i]);
    }
  }
}

void __not_in_flash_func(SynthMode::handleNoteOn)(byte channel, byte pitch, byte velocity) {
  if ((channel - 1) == PRA32_U_MIDI_CH) {
    g_synth.note_on(pitch, velocity);
  }
}

void __not_in_flash_func(SynthMode::handleNoteOff)(byte channel, byte pitch, byte velocity) {
  if ((channel - 1) == PRA32_U_MIDI_CH) {
    (void)velocity;
    g_synth.note_off(pitch);
  }
}

void __not_in_flash_func(SynthMode::handleControlChange)(byte channel, byte number, byte value) {
  if ((channel - 1) == PRA32_U_MIDI_CH) {
    g_synth.control_change(number, value);
  }
}

void __not_in_flash_func(SynthMode::handleProgramChange)(byte channel, byte number) {
  if ((channel - 1) == PRA32_U_MIDI_CH) {
    g_synth.program_change(number);
  }
}

void __not_in_flash_func(SynthMode::handlePitchBend)(byte channel, int bend) {
  if ((channel - 1) == PRA32_U_MIDI_CH) {
    g_synth.pitch_bend((bend + 8192) & 0x7F, (bend + 8192) >> 7);
  }
}

void __not_in_flash_func(SynthMode::handleClock)() {}
void __not_in_flash_func(SynthMode::handleStart)() {}
void __not_in_flash_func(SynthMode::handleStop)() {}

volatile uint8_t lastSettings[128] = { 0 };


void __not_in_flash_func(SynthMode::updateAll)() {
  uint8_t toUpdate[] = { 74, 71, 92 };
  for (uint8_t i = 0; i < sizeof(toUpdate); i++) {
    g_synth.control_change(toUpdate[i], lastSettings[toUpdate[i]]);
  }
}


void __not_in_flash_func(SynthMode::handleKnob)(uint8_t knob, uint8_t value) {
  switch (knob) {
    case 0:
      g_synth.program_change(value / 8);
      updateAll();
      break;
    case 1:
      g_synth.control_change(74, value);
      lastSettings[74] = value;
      break;
    case 2:
      g_synth.control_change(71, value);
      lastSettings[71] = value;
      break;
    case 3:
      g_synth.control_change(92, value);
      lastSettings[92] = value;
      break;
  }
}

void __not_in_flash_func(SynthMode::handleTouch)(uint8_t pad, uint8_t value) {
  if (value) {
    g_synth.note_on(50 + pad, 127);
  } else {
    g_synth.note_off(50 + pad);
  }
}

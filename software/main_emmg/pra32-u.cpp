/*
 * Digital Synth PRA32-U
 */


////////////////////////////////////////////////////////////////

#include "pra32-u.h"


// This is a bad dependency
uint8_t g_midi_ch = PRA32_U_MIDI_CH;

#include "pra32-u/pra32-u-common.h"
#include "pra32-u/pra32-u-synth.h"



PRA32_U_Synth g_synth;


I2S g_i2s_output(OUTPUT);

static volatile uint32_t s_debug_measurement_elapsed0_us = 0;
static volatile uint32_t s_debug_measurement_max0_us = 0;
static volatile uint32_t s_debug_measurement_elapsed1_us = 0;
static volatile uint32_t s_debug_measurement_max1_us = 0;

void handleNoteOn(byte channel, byte pitch, byte velocity);
void handleNoteOff(byte channel, byte pitch, byte velocity);
void handleControlChange(byte channel, byte number, byte value);
void handleProgramChange(byte channel, byte number);
void handlePitchBend(byte channel, int bend);
void handleClock();
void handleStart();
void handleStop();
void writeProgramsToFlashAndEndSketch();

void __not_in_flash_func(synth_setup_core1)() {}

void __not_in_flash_func(synth_loop1)() {
  boolean processed = g_synth.secondary_core_process();
  if (processed) {
    static uint32_t s_loop_counter = 0;
    s_loop_counter++;
    if (s_loop_counter >= 16 * 400) {
      s_loop_counter = 0;
    }
  }
}

void __not_in_flash_func(synth_setup)() {
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
}

void __not_in_flash_func(synth_loop)(void (*poll)()) {
  //for (uint32_t i = 0; i < ((PRA32_U_I2S_BUFFER_WORDS + 31) / 32) + 1; i++) {
  (*poll)();
  //}

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

namespace PRA32_U {
void __not_in_flash_func(handleNoteOn)(byte channel, byte pitch, byte velocity) {
  if ((channel - 1) == g_midi_ch) {
    g_synth.note_on(pitch, velocity);
  }
}

void __not_in_flash_func(handleNoteOff)(byte channel, byte pitch, byte velocity) {
  if ((channel - 1) == g_midi_ch) {
    (void)velocity;
    g_synth.note_off(pitch);
  }
}

void __not_in_flash_func(handleControlChange)(byte channel, byte number, byte value) {
  if ((channel - 1) == g_midi_ch) {
    g_synth.control_change(number, value);
  }
}

void __not_in_flash_func(handleProgramChange)(byte channel, byte number) {
  if ((channel - 1) == g_midi_ch) {
    g_synth.program_change(number);
  }
}

void __not_in_flash_func(handlePitchBend)(byte channel, int bend) {
  if ((channel - 1) == g_midi_ch) {
    g_synth.pitch_bend((bend + 8192) & 0x7F, (bend + 8192) >> 7);
  }
}

void __not_in_flash_func(handleClock)() {}
void __not_in_flash_func(handleStart)() {}
void __not_in_flash_func(handleStop)() {}

void __not_in_flash_func(handleController)(uint8_t knob, uint8_t value) {
  switch (knob) {
    case 0:
      g_synth.program_change(value / 8);
      break;
    case 1:
      g_synth.control_change(74, value);
      break;
    case 2:
      g_synth.control_change(71, value);
      break;
    case 3:
      g_synth.control_change(92, value);
      break;
  }
}

void __not_in_flash_func(handleTouch)(uint8_t pad, uint8_t value) {
  if (value) {
    g_synth.note_on(50 + pad, 127);
  } else {
    g_synth.note_off(50 + pad);
  }
}
}
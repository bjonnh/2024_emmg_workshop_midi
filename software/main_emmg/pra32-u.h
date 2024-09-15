#ifndef PRA32U_MAIN_H
#define PRA32U_MAIN_H

#define PRA32_U_VERSION "v2.5.1    "

//#define PRA32_U_USE_DEBUG_PRINT               // Serial1

#define PRA32_U_USE_USB_MIDI  // Select USB Stack: "Adafruit TinyUSB" in the Arduino IDE "Tools" menu

//#define PRA32_U_USE_UART_MIDI                 // Serial2

#define PRA32_U_UART_MIDI_SPEED (31250)
//#define PRA32_U_UART_MIDI_SPEED               (38400)

#define PRA32_U_UART_MIDI_TX_PIN (0)
#define PRA32_U_UART_MIDI_RX_PIN (1)

#define PRA32_U_MIDI_CH (0)  // 0-based

// for Pimoroni Pico Audio Pack (PIM544)
//#define PRA32_U_I2S_DAC_MUTE_OFF_PIN          (22)
#define PRA32_U_I2S_DATA_PIN (22)
//#define PRA32_U_I2S_MCLK_PIN                  (0)
//#define PRA32_U_I2S_MCLK_MULT                 (0)
#define PRA32_U_I2S_BCLK_PIN (20)  // LRCLK Pin is PRA32_U_I2S_BCLK_PIN + 1
#define PRA32_U_I2S_SWAP_BCLK_AND_LRCLK_PINS (true)
#define PRA32_U_I2S_SWAP_LEFT_AND_RIGHT (false)

#define PRA32_U_I2S_BUFFERS (4)
#define PRA32_U_I2S_BUFFER_WORDS (256)

//#define PRA32_U_USE_PWM_AUDIO_INSTEAD_OF_I2S

// for Pimoroni Pico VGA Demo Base (PIM553)
//#define PRA32_U_PWM_AUDIO_L_PIN               (28)
//#define PRA32_U_PWM_AUDIO_R_PIN               (27)

#define PRA32_U_USE_2_CORES_FOR_SIGNAL_PROCESSING

#define PRA32_U_USE_EMULATED_EEPROM

////////////////////////////////////////////////////////////////

//#define PRA32_U_USE_CONTROL_PANEL               // Experimental

#define PRA32_U_USE_CONTROL_PANEL_KEY_INPUT  // Use tactile switches
#define PRA32_U_KEY_INPUT_ACTIVE_LEVEL (HIGH)
#define PRA32_U_KEY_INPUT_PIN_MODE (INPUT_PULLDOWN)
#define PRA32_U_KEY_INPUT_PREV_KEY_PIN (16)
#define PRA32_U_KEY_INPUT_NEXT_KEY_PIN (18)
#define PRA32_U_KEY_INPUT_PLAY_KEY_PIN (20)
#define PRA32_U_KEY_ANTI_CHATTERING_WAIT (15)
#define PRA32_U_KEY_LONG_PRESS_WAIT (375)

#define PRA32_U_USE_CONTROL_PANEL_ANALOG_INPUT  // Use ADC0, ADC1, and ADC2
#define PRA32_U_ANALOG_INPUT_REVERSED (true)
#define PRA32_U_ANALOG_INPUT_CORRECTION (-504)
#define PRA32_U_ANALOG_INPUT_THRESHOLD (504)
#define PRA32_U_ANALOG_INPUT_DENOMINATOR (504)

#define PRA32_U_USE_CONTROL_PANEL_OLED_DISPLAY  // Use SSD1306 monochrome 128x64 OLED
#define PRA32_U_OLED_DISPLAY_I2C (i2c1)
#define PRA32_U_OLED_DISPLAY_I2C_SDA_PIN (6)
#define PRA32_U_OLED_DISPLAY_I2C_SCL_PIN (7)
#define PRA32_U_OLED_DISPLAY_I2C_ADDRESS (0x3C)
#define PRA32_U_OLED_DISPLAY_CONTRAST (0xFF)
#define PRA32_U_OLED_DISPLAY_ROTATION (true)

#include <Arduino.h>
#include <MIDI.h>
#include <I2S.h>

void __not_in_flash_func(synth_setup)();
void __not_in_flash_func(synth_setup_core1)();
void __not_in_flash_func(synth_loop)();
void __not_in_flash_func(synth_loop1)();

namespace PRA32_U {
void __not_in_flash_func(handleNoteOn)(byte channel, byte pitch, byte velocity);
void __not_in_flash_func(handleNoteOff)(byte channel, byte pitch, byte velocity);
void __not_in_flash_func(handleControlChange)(byte channel, byte number, byte value);
void __not_in_flash_func(handleProgramChange)(byte channel, byte number);
void __not_in_flash_func(handlePitchBend)(byte channel, int bend);
void __not_in_flash_func(handleController)(uint8_t knob, uint8_t value);
void __not_in_flash_func(handleTouch)(uint8_t knob, uint8_t value);
void __not_in_flash_func(handleClock)();
void __not_in_flash_func(handleStart)();
void __not_in_flash_func(handleStop)();
}

#endif

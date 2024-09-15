#ifndef PRA32U_MAIN_H
#define PRA32U_MAIN_H

#define PRA32_U_VERSION "v2.5.1    "

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
#define PRA32_U_I2S_BUFFER_WORDS (196)

#define PRA32_U_USE_2_CORES_FOR_SIGNAL_PROCESSING

#define PRA32_U_USE_EMULATED_EEPROM


#endif

# EMMG MIDI Controller/Synthesizer Project

## Overview
This is an Arduino-based MIDI controller and synthesizer project using the RP2040 microcontroller (Raspberry Pi Pico). The device can operate in two modes:
1. **Controller Mode** - Acts as a MIDI controller with knobs and pads
2. **Synthesizer Mode** - Functions as a digital synthesizer based on the PRA32-U engine

## Target Platform
- **Board**: RP2040 (Raspberry Pi Pico) - VCC GND YD-RP2040
- **Clock Speed**: 200MHz (overclocked for better synthesizer performance)
- **Build System**: Arduino CLI

## Project Structure

### Core Files
- `main_emmg.ino` - Main Arduino sketch entry point that handles boot mode selection and initializes the device
- `config.h` - Hardware configuration (pin assignments, button/knob/pad counts)
- `sketch.yaml` - Arduino build configuration
- `compile.sh` - Build script that installs dependencies and compiles the project

### Hardware Interface Components
- `device.h/cpp` - Main device state management class
- `knobs.h/cpp` - Analog knob input handling with hysteresis filtering
- `touch.h/cpp` - Touch pad input handling (12 pads)
- `debouncer.h/cpp` - Button/input debouncing logic
- `display.h/cpp` - OLED display interface (Adafruit SSD1306)

### Operating Modes
- `controller.h/cpp` - MIDI controller mode implementation
- `controllersettings.h/cpp` - Settings and configuration for controller mode
- `synth.h/cpp` - Synthesizer mode implementation

### MIDI and Audio
- `piomidi.h/cpp` - MIDI interface using PIO (Programmable I/O)
- `pra32-u.h` - Main header for the PRA32-U synthesizer engine
- `pra32-u/` - Directory containing the synthesizer engine implementation

### Storage
- `storage.h/cpp` - EEPROM storage management for presets and settings

### Utilities
- `debug.h` - Debug macros and utilities

## Synthesizer Engine (PRA32-U)
The synthesizer engine is based on the [digital-synth-pra32-u](https://github.com/risgk/digital-synth-pra32-u) project and includes:

### Core Components
- `pra32-u-synth.h` - Main synthesizer class
- `pra32-u-osc.h` - Oscillator implementation
- `pra32-u-filter.h` - Filter implementation
- `pra32-u-eg.h` - Envelope generator
- `pra32-u-lfo.h` - Low-frequency oscillator
- `pra32-u-amp.h` - Amplifier stage
- `pra32-u-noise-gen.h` - Noise generator

### Effects
- `pra32-u-chorus-fx.h` - Chorus effect
- `pra32-u-delay-fx.h` - Delay effect

### Tables and Constants
- `pra32-u-*-table.h` - Various lookup tables for oscillators, filters, etc.
- `pra32-u-constants.h` - Synthesizer constants
- `pra32-u-program-table.h` - Preset programs

### Utilities
- `pra32-u-generate-*.rb` - Ruby scripts for generating lookup tables
- `pra32-u-control-panel.h` - Control panel interface
- `pra32-u-control-panel-*-table.h` - Font and page tables for UI

## Hardware Configuration

### Inputs
- **8 Analog Knobs**: Multiplexed through address pins [2,3,4] to analog pin A2
- **12 Touch Pads**: Connected to pins [14,11,8,16,15,12,9,6,13,10,7,17]
- **1 Button**: Connected to pin 5

### External Libraries Required
- MIDIUSB - USB MIDI support
- MIDI Library - MIDI protocol handling
- Adafruit GFX Library - Graphics primitives
- Adafruit SSD1306 - OLED display driver

## Build Instructions
1. Install Arduino CLI
2. Run `./compile.sh` to install dependencies and compile
3. Upload to RP2040 board

## Boot Mode Selection
The device checks the button state at startup to determine which mode to boot into:
- Button pressed: Controller Mode
- Button not pressed: Synthesizer Mode

## Key Features
- Dual-mode operation (MIDI controller or synthesizer)
- 8 knobs with hysteresis filtering for stable analog readings
- 12 velocity-sensitive touch pads
- OLED display for visual feedback
- USB MIDI support
- Hardware MIDI I/O via PIO
- Preset storage in EEPROM
- Overclocked to 200MHz for improved synthesizer performance

## External Dependencies
- Hysteresis filter implementation from [Control-Surface](https://github.com/tttapa/Control-Surface)
- PRA32-U synthesizer engine from [digital-synth-pra32-u](https://github.com/risgk/digital-synth-pra32-u)

## Performance Considerations
- Functions that are performance-critical or called frequently in the audio processing loop should be marked with `__not_in_flash_func` to ensure they run from RAM instead of flash memory
- This is especially important for MIDI event handlers and audio processing functions
- Examples: `handleNoteOn`, `handleNoteOff`, `handleControlChange`, `loop`, `loop1`

## License
MIT License (Copyright 2024 Jonathan Bisson)
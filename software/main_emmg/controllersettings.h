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

#ifndef EMMG_CONTROLLER_SETTINGS_H
#define EMMG_CONTROLLER_SETTINGS_H

#include <Arduino.h>
#include "config.h"


#define SETTINGS_SIZE 255

class Controller {
public:
  Controller(){};
  Controller(uint8_t cc, uint8_t channel);
  uint8_t cc = 0;
  uint8_t channel = 1;
};


enum class PadType : uint8_t {
  NOTE,
  CC,
  CC_LATCH
};

constexpr const char* to_string(PadType type) {
  switch (type) {
    case PadType::NOTE: return "Note";
    case PadType::CC: return "CC";
    case PadType::CC_LATCH: return "CCL";
    default: return "Unknown";
  }
};

class Pad {
public:
  Pad(){};
  Pad(uint8_t cc, uint8_t channel, uint8_t current, PadType type);
  uint8_t cc = 0;  // we call it cc but it can be a note too
  uint8_t channel = 1;
  uint8_t current = 0; // Used for latching
  PadType type = PadType::NOTE;
};

class ControllerSettings {
public:
  void saveToArray(uint8_t* arr);
  void loadFromArray(const uint8_t* arr);

  Controller controllers[NUMBER_OF_KNOBS];
  Pad pads[NUMBER_OF_PADS];
};

#endif  // EMMG_CONTROLLER_SETTINGS_H

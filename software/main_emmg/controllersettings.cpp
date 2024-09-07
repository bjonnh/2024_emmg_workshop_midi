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

#include "controllersettings.h"
#include "debug.h"

Controller::Controller(uint8_t cc, uint8_t channel)
  : cc(cc), channel(channel){};

Pad::Pad(uint8_t cc, uint8_t channel, uint8_t current, PadType type)
  : cc(cc), channel(channel), current(0), type(type){};

void ControllerSettings::saveToArray(uint8_t* arr) {
  // We store the knobs at 0
  for (int i = 0; i < NUMBER_OF_KNOBS; i++) {
    arr[i * 2] = controllers[i].cc;
    arr[i * 2 + 1] = controllers[i].channel;
  }
  // We store the pads at 128
  for (int i = 0; i < NUMBER_OF_PADS; i++) {
    arr[128 + i * 3] = pads[i].cc;
    arr[128 + i * 3 + 1] = pads[i].channel;
    arr[128 + i * 3 + 2] = static_cast<uint8_t>(pads[i].type);
  }
}

void ControllerSettings::loadFromArray(const uint8_t* arr) {
  // Load knob settings
  for (int i = 0; i < NUMBER_OF_KNOBS; i++) {
    controllers[i].cc = arr[i * 2];
    controllers[i].channel = arr[i * 2 + 1];
  }

  // Load pad settings
  for (int i = 0; i < NUMBER_OF_PADS; i++) {
    pads[i].cc = arr[128 + i * 3];
    pads[i].channel = arr[128 + i * 3 + 1];
    pads[i].type = static_cast<PadType>(arr[128 + i * 3 + 2]);
    SERIAL_PRINT("Pad ");
    SERIAL_PRINTLN(i);
    SERIAL_PRINT("CC "); 
    SERIAL_PRINT(pads[i].cc);
    SERIAL_PRINT(" CH ");
    SERIAL_PRINT(pads[i].channel);
    SERIAL_PRINT(" Type ");
    SERIAL_PRINT(to_string(pads[i].type));
    SERIAL_PRINT(" Type_num ");
    SERIAL_PRINTLN(arr[128 + i * 3 + 2]);
  }
}
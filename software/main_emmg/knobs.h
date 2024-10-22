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


#ifndef KNOBS_H
#define KNOBS_H

#include <Arduino.h>
#include <hardware/adc.h>
#include "config.h"
#include "debug.h"

class LowPassFilter {
public:
  LowPassFilter(float alpha = 0.1, int delta = 3);
  uint8_t apply(int value);

private:
  float alpha;
  int delta;
  float filtered_value;
};

class Knobs {
public:
  Knobs();
  void begin();
  void tick();
  int read(int knob);
  int* getValues();
  int getSize();

private:
  int direct_read(int knob);
  int num_address_pins;
  int num_knobs;
  LowPassFilter* filters[NUMBER_OF_KNOBS];
  int values[NUMBER_OF_KNOBS];
  uint8_t knobs_analog_pin = KNOBS_ANALOG_PIN;
  uint8_t knobs_analog_pin_index = KNOBS_ANALOG_PIN_INDEX;
  static constexpr uint8_t address_pins[KNOBS_ADDRESS_SIZE] = KNOBS_PINS;
  static constexpr int addresses[NUMBER_OF_KNOBS][KNOBS_ADDRESS_SIZE] = KNOBS_ADDRESSES;
};

#endif  // KNOBS_H

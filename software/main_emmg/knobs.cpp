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


#include "knobs.h"

// LowPassFilter implementation
// alpha is a smoothing factor, 0 < alpha < 1, the lowest the more it filters but the slower it responds
// delta is the minimum difference between values to consider an immediate change skipping the filter
LowPassFilter::LowPassFilter(float alpha, int delta)
  : alpha(alpha), delta(delta), filtered_value(-1) {}

uint8_t LowPassFilter::apply(int value) {
  if (filtered_value == -1) {
    filtered_value = value;
  } else {
    if (abs(value - filtered_value) > delta) {
      filtered_value = value;
    } else if (abs(value - filtered_value) >= 1) {  // A subtle rounding error happened because of floats, maybe we don't want floats all?
      filtered_value = alpha * value + (1 - alpha) * filtered_value;
    }
  }
  return filtered_value;
}

// Knobs implementation
Knobs::Knobs()
  : num_address_pins(KNOBS_ADDRESS_SIZE), num_knobs(NUMBER_OF_KNOBS) {
}

void Knobs::begin() {
  adc_init();
  adc_gpio_init(knobs_analog_pin);
  for (int i = 0; i < num_knobs; i++) {
    filters[i] = new LowPassFilter();
  }
  analogReadResolution(12);
  for (int i = 0; i < num_address_pins; i++) {
    gpio_init(address_pins[i]);
    gpio_set_dir(address_pins[i], GPIO_OUT);
  }
}

void __not_in_flash_func(Knobs::tick)() {
  adc_select_input(knobs_analog_pin_index);
  for (int i = 0; i < num_knobs; i++) {
    direct_read(i);
  }
}

int Knobs::getSize() {
  return num_knobs;
}

int* Knobs::getValues() {
  return values;
}

int Knobs::read(int knob) {
  adc_select_input(knobs_analog_pin_index);
  return direct_read(knob);
}

inline int Knobs::direct_read(int knob) {
  if (knob >= num_knobs) {
    SERIAL_PRINTLN("Knob index out of range");
    return -1;
  }
  for (int j = 0; j < num_address_pins; j++) {
    gpio_put(address_pins[j], addresses[knob][j]);
  }
  delayMicroseconds(5);
  int value = adc_read();

  int raw_value = 130 - map(value, 100, 3900, 0, 130);  // 12-bit ADC max value is 4095
  if (raw_value < 0) raw_value = 0;
  uint8_t filtered_value = filters[knob]->apply(raw_value);
  if (filtered_value > 127) filtered_value = 127;
  values[knob] = filtered_value;
  return values[knob];
}

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

class HysteresisFilter {
public:
  HysteresisFilter(uint16_t threshold);

  uint8_t apply(uint16_t value);

private:
  uint16_t threshold;
  uint8_t previous_value;
};


template <uint8_t Bits, class T_in = uint16_t, class T_out = uint8_t>
class Hysteresis {
  public:
    /**
     * @brief   Update the hysteresis output with a new input value.
     *
     * @param   inputLevel
     *          The input to calculate the output level from.
     * @retval  true
     *          The output level has changed.
     * @retval  false
     *          The output level is still the same.
     */
    bool update(T_in inputLevel) {
        T_in prevLevelFull = (T_in(prevLevel) << Bits) | offset;
        T_in lowerbound = prevLevel > 0 ? prevLevelFull - margin : 0;
        T_in upperbound = prevLevel < max_out ? prevLevelFull + margin : max_in;
        if (inputLevel < lowerbound || inputLevel > upperbound) {
            setValue(inputLevel);
            return true;
        }
        return false;
    }

    /**
     * @brief   Get the current output level.
     *
     * @return  The output level.
     */
    T_out getValue() const { return prevLevel; }

    /** 
     * @brief   Forcefully update the internal state to the given level.
     */
    void setValue(T_in inputLevel) { prevLevel = inputLevel >> Bits; }

  private:
    T_out prevLevel = 0;
    constexpr static T_in margin = (1ul << Bits) - 1ul;
    constexpr static T_in offset = Bits >= 1 ? 1ul << (Bits - 1) : 0;
    constexpr static T_in max_in = static_cast<T_in>(-1);
    constexpr static T_out max_out = static_cast<T_out>(max_in >> Bits);
    static_assert(max_in > 0, "Error: only unsigned types are supported");
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
  Hysteresis<7, uint16_t, uint8_t>* filters[NUMBER_OF_KNOBS];
  int values[NUMBER_OF_KNOBS];
  uint8_t knobs_analog_pin = KNOBS_ANALOG_PIN;
  uint8_t knobs_analog_pin_index = KNOBS_ANALOG_PIN_INDEX;
  static constexpr uint8_t address_pins[KNOBS_ADDRESS_SIZE] = KNOBS_PINS;
  static constexpr int addresses[NUMBER_OF_KNOBS][KNOBS_ADDRESS_SIZE] = KNOBS_ADDRESSES;
};

#endif  // KNOBS_H

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

#include "touch.h"

Touch::Touch(int pin, unsigned long threshold)
    : touchPin(pin), touchThreshold(threshold) {
    pinMode(touchPin, INPUT);
}

bool Touch::isTouched() {
    uint16_t capacitance = readCapacitance();
    return capacitance > touchThreshold;
}

void Touch::calibrate() {
    // Calibrate the touch sensor to set a proper threshold
    uint16_t baseline = 0;
    
    baseline = readCapacitance();
    touchThreshold = baseline * 1.2 + 100; // Add some margin
}

uint16_t Touch::readCapacitance() {
    uint16_t ticks = 0;
    for (uint8_t sample = 0 ; sample < numSamples ; sample++) {
      pinMode(touchPin, OUTPUT);
      digitalWrite(touchPin, HIGH);
      delayMicroseconds(10); // Charge the pin
  
      pinMode(touchPin, INPUT);
      
      while (digitalRead(touchPin) == HIGH) {
          if (ticks >= 10000) { 
           return 10000;
          }
          ticks++;
      }
    }
    return ticks;
}

MultiTouch::MultiTouch()  {
    sensors = new Touch*[sensorCount];
    touches = new bool[sensorCount];
    for (int i = 0; i < sensorCount; i++) {
        sensors[i] = new Touch(sensorPins[i], 50);
        touches[i] = false;
    }
}

void MultiTouch::calibrateAll() {
    for (int i = 0; i < sensorCount; i++) {
        sensors[i]->calibrate();
    }
}

bool MultiTouch::isTouched(int index) {
    if (index < 0 || index >= sensorCount) {
        return false;
    }
    return sensors[index]->isTouched();
}

int MultiTouch::getFirstTouch() {
    tick();
    for (int i = 0; i < sensorCount; i++) {
        if (touches[i]) {
            return i;
        }
    }
    return -1;
}

void MultiTouch::tick() {
    for (int i = 0; i < sensorCount; i++) {
        bool newTouch = sensors[i]->isTouched();
        touches[i] = newTouch;
    }
}

bool *MultiTouch::getTouches() {
  return touches;
}
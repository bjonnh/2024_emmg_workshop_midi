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


#ifndef TOUCH_H
#define TOUCH_H

#include <Arduino.h>

class Touch {
public:
    Touch(int pin, unsigned long threshold);
    bool isTouched();
    void calibrate();

private:
    int touchPin;
    uint16_t touchThreshold;
    uint16_t readCapacitance();
    uint8_t numSamples = 10;
};

class MultiTouch {
public:
    MultiTouch();
    void calibrateAll();
    bool isTouched(int index);
    int getFirstTouch();
    bool *getTouches();
    void tick();
private:
    Touch **sensors;
    int sensorCount = 12;
    static constexpr int sensorPins[12] = {14, 11, 8, 16, 15, 12, 9, 6, 13, 10, 7, 17};
    bool *touches;
};

#endif

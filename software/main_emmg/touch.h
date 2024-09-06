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

#ifndef KNOBS_H
#define KNOBS_H

#include <Arduino.h>
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
    void tick();
    int read(int knob);
    int *getValues();
    int getSize();

private:
    int num_address_pins;
    int num_knobs;
    LowPassFilter* filters[8];
    int values[8]; 
    uint8_t knobs_analog_pin = A2;
    static constexpr uint8_t address_pins[3] = {2, 3, 4};
    static constexpr int addresses[8][3] = {{1,1,1}, {1,0,1}, {1,1,0}, {0,0,0}, {0,0,1}, {0,1,1}, {0,1,0}, {1,0,0}};

};

#endif  // KNOBS_H

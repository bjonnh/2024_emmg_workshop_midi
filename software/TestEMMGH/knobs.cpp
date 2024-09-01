#include "knobs.h"

// LowPassFilter implementation
// alpha is a smoothing factor, 0 < alpha < 1, the lowest the more it filters but the slower it responds
// delta is the minimum difference between values to consider an immediate change skipping the filter
LowPassFilter::LowPassFilter(float alpha, int delta) 
    : alpha(alpha), delta(delta), filtered_value(-1) {}

int LowPassFilter::apply(int value) {
    if (filtered_value == -1) {
        filtered_value = value;
    } else {
        if (abs(value - filtered_value) > delta) {
            filtered_value = value;
        } else {
            filtered_value = alpha * value + (1 - alpha) * filtered_value;
        }
    }
    return filtered_value;
}

// Knobs implementation
Knobs::Knobs()    : num_address_pins(3), num_knobs(8) {
    for (int i = 0; i < num_knobs; i++) {
        filters[i] = new LowPassFilter();
    }
    analogReadResolution(12);
    for (int i = 0; i < num_address_pins; i++) {
        pinMode(address_pins[i], OUTPUT);
    }

}

void Knobs::tick() {
    for (int i = 0; i < num_knobs; i++) {
        read(i);
    }
}

int Knobs::get_size() {
  return num_knobs;
}

int *Knobs::get_values() {
  return values;
}

int Knobs::read(int knob) {
    if (knob >= num_knobs) {
        Serial.println("Knob index out of range");
        return -1;
    }
    for (int j = 0; j < num_address_pins; j++) {
        digitalWrite(address_pins[j], addresses[knob][j]);
        
    }
    delay(1);
    int raw_value = (4096 - analogRead(knobs_analog_pin)) / 32;  // 12-bit ADC max value is 4095
    values[knob] = filters[knob]->apply(raw_value);
    return values[knob];
}

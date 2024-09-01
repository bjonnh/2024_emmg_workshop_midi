#ifndef DEVICESTATE_H
#define DEVICESTATE_H

#include <Arduino.h>

class Device {
public:
    Device();

    void begin();
    void poll();
    bool isUpdated();

    int getKnobValue(uint8_t index) const;
    bool getButtonValue() const;
    bool getTouchPadValue(uint8_t index) const;

private:
    int knobValues[8];
    bool buttonState;
    bool touchPadStates[12];
    bool updated;
};

#endif // DEVICESTATE_H

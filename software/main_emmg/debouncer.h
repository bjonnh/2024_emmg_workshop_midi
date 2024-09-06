#ifndef DEBOUNCER_H
#define DEBOUNCER_H

#include <Arduino.h>

class Debouncer {
public:
    Debouncer(int pin, unsigned long debounceDelay = 50);
    void tick();
    bool isPressed();

private:
    int pin;
    unsigned long debounceDelay;
    int buttonState;
    int lastButtonState;
    unsigned long lastDebounceTime;
    bool buttonPressed;
};

#endif // DEBOUNCER_H

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

MultiTouch::MultiTouch() 
    : onTouch(nullptr), onRelease(nullptr) {
    sensors = new Touch*[sensorCount];
    oldTouches = new bool[sensorCount];
    for (int i = 0; i < sensorCount; i++) {
        sensors[i] = new Touch(sensorPins[i], 50);
        oldTouches[i] = false;
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
        if (oldTouches[i]) {
            return i;
        }
    }
    return -1;
}

void MultiTouch::tick() {
    for (int i = 0; i < sensorCount; i++) {
        bool newTouch = sensors[i]->isTouched();
        if (newTouch != oldTouches[i]) {
            if (newTouch && onTouch != nullptr) {
                onTouch(i);
            }
            if (!newTouch && onRelease != nullptr) {
                onRelease(i);
            }
            oldTouches[i] = newTouch;
        }
    }
}

void MultiTouch::setOnTouch(void (*callback)(int)) {
    onTouch = callback;
}

void MultiTouch::setOnRelease(void (*callback)(int)) {
    onRelease = callback;
}

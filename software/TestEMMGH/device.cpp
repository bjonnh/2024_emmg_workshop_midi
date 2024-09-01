#include "device.h"
#include "knobs.h"
#include "touch.h"

Knobs knobs;
MultiTouch touch;

Device::Device() : buttonState(false), updated(false) {
  for (int i = 0; i < 8; i++) {
    knobValues[i] = 0;
  }
  for (int i = 0; i < 12; i++) {
    touchPadStates[i] = false;
  }
}

void Device::begin() {
  touch.calibrateAll();
}

void Device::poll() {
  knobs.tick();
  //touch.tick(); // no need to tick until we have the callbacks wired

  const int* values = knobs.get_values();
  for (uint8_t i = 0; i < knobs.get_size(); i++) {
    uint8_t value = values[i];
    if (knobValues[i] != value) {
      knobValues[i] = value;
      updated = true;
    }
  }

  for (uint8_t i = 0; i < 12; i++) {
    bool touched = touch.isTouched(i);
    if (touchPadStates[i] != touched) {
      touchPadStates[i] = touched;
      updated = true;
    }
  }
}


bool Device::isUpdated() {
  bool wasUpdated = updated;
  updated = false;
  return wasUpdated;
}

int Device::getKnobValue(uint8_t index) const {
  if (index < 8) {
    return knobValues[index];
  }
  return -1; // Error value
}

bool Device::getButtonValue() const {
  return buttonState;
}

bool Device::getTouchPadValue(uint8_t index) const {
  if (index < 12) {
    return touchPadStates[index];
  }
  return false; // Error value
}

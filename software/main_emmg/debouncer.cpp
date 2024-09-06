#include "debouncer.h"

Debouncer::Debouncer(int pin, unsigned long debounceDelay)
  : pin(pin), debounceDelay(debounceDelay), buttonState(LOW), lastButtonState(LOW), lastDebounceTime(0), buttonPressed(false) {

}

void Debouncer::tick() {
  pinMode(pin, INPUT_PULLUP);
  int reading = !digitalRead(pin);


  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        buttonPressed = true;
      } else {
        buttonPressed = false;
      }
    }
  }

  lastButtonState = reading;
}

bool Debouncer::isPressed() {
  return buttonPressed;
}

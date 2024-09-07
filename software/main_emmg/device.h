#ifndef DEVICESTATE_H
#define DEVICESTATE_H

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include "debouncer.h"
#include "debug.h"
#include "display.h"
#include "piomidi.h"

#define BUTTON_PIN 5

class Device {
  public:
    Device(midi::MidiInterface<midi::SerialMIDI<Adafruit_USBD_MIDI> >& midi,
              midi::MidiInterface<midi::PioMIDI>& midi_1 );

    void begin();
    void poll();
    bool isUpdated();

    int getKnobValue(uint8_t index) const;
    bool getButtonValue() const;
    bool getTouchPadValue(uint8_t index) const;
    void setKnobCallback(void (*callback)(uint8_t, uint8_t));
    void setTouchPadCallback(void (*callback)(uint8_t, uint8_t));
    void setButtonCallback(void (*callback)(uint8_t, uint8_t));
    void sendControlChange(uint8_t, uint8_t, uint8_t);
    void sendNoteOn(uint8_t, uint8_t, uint8_t);
    void sendNoteOff(uint8_t, uint8_t, uint8_t);
    void midiPanic();

    Display display;
  private:
    int knobValues[8];
    bool buttonState;
    Debouncer debounced_button;
    bool touchPadStates[12];
    void (*knobCallback)(uint8_t, uint8_t);
    void (*touchPadCallback)(uint8_t, uint8_t);
    void (*buttonCallback)(uint8_t, uint8_t);

    void triggerKnobCallback(uint8_t index, uint8_t value);
    void triggerTouchPadCallback(uint8_t index, uint8_t value);
    void triggerButtonCallback(uint8_t index, uint8_t value);
    midi::MidiInterface<midi::SerialMIDI<Adafruit_USBD_MIDI> >& MIDI;
    midi::MidiInterface<midi::PioMIDI>& MIDI_1;
};

#endif // DEVICESTATE_H

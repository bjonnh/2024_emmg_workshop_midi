#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <Wire.h>

#include "controller.h"
#include "device.h"
#include "display.h"
#include "piomidi.h"

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);
PIO_SERIAL_MIDI(1, 0, 1);
Device device(MIDI, MIDI_IF_1);
ControllerMode controller(device);

void setup()
{
  USBDevice.setManufacturerDescriptor("bjonnh.net                     ");
  USBDevice.setProductDescriptor         ("EMMG MIDI 2024 @PS1           ");
  
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }
  //usb_midi.setStringDescriptor("EMMG MIDI 2024");
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI_IF_1.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(9600);
  //while (!Serial);
  //delay(1);
  SERIAL_PRINTLN("Starting up");
  SERIAL_PRINTLN("Booting device");
  device.begin();
}

void loop()
{
  if (!Serial) { Serial.begin(9600); }
  #ifdef TINYUSB_NEED_POLLING_TASK
  TinyUSBDevice.task();
  #endif
  if (!TinyUSBDevice.mounted()) {
    return;
  }
  controller.loop();
}

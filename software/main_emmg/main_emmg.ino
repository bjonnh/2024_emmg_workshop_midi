/*
 * Copyright (c) 2024. Jonathan Bisson
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include <Wire.h>
#include <EEPROM.h>

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
  EEPROM.begin(1024);
  USBDevice.setManufacturerDescriptor("bjonnh.net                     ");
  USBDevice.setProductDescriptor         ("EMMG MIDI 2024 @PS1           ");
  
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }
  //usb_midi.setStringDescriptor("EMMG MIDI 2024");
  MIDI.begin(MIDI_CHANNEL_OMNI);
  MIDI_IF_1.begin(MIDI_CHANNEL_OMNI);
  Serial.begin(9600);
  while (!Serial);
  //delay(1);
  SERIAL_PRINTLN("Starting up");
  SERIAL_PRINTLN("Booting device");
  device.begin();
  controller.begin();
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

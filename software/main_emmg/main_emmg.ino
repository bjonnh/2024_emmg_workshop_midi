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
Device device;
ControllerMode controller(device);


volatile bool core_0_ready = false;

uint8_t serial_tries = 10;
void setup() {
  MIDI.begin(MIDI_CHANNEL_OMNI);

  Serial.begin(9600);
  while (serial_tries > 0) {
    if (Serial) {
      serial_tries = 0;
    } else {
      serial_tries--;
      delay(100);
    }
  }
  SERIAL_PRINTLN("Starting up");
  SERIAL_PRINTLN("Starting MIDI USB");
  USBDevice.setManufacturerDescriptor("bjonnh.net                     ");
  USBDevice.setProductDescriptor("EMMG MIDI 2024 @PS1           ");

  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }
  //while (!TinyUSBDevice.mounted()) {
  //SERIAL_PRINTLN("Waiting for device to mound");
  // delay(100);
  // }
  SERIAL_PRINTLN("Booting device");
  Wire1.setSDA(SCREEN_SDA);
  Wire1.setSCL(SCREEN_SCL);
  Wire1.setClock(1000000);
  Wire1.begin();
  device.MIDI = &MIDI;
  device.begin();
  SERIAL_PRINTLN("Booting controller mode");

  controller.begin();
  SERIAL_PRINTLN("Starting core 1");
  core_0_ready = true;
}

void setup1() {
  while (!core_0_ready) { delay(10); }
}

void loop() {
  if (!Serial) { Serial.begin(9600); }

  controller.loop();
}

void loop1() {
  // YOU ARE NOT ALLOWED TO MODIFY ANYTHING IN THERE EXCEPT clear_updated
  controller.update();
}

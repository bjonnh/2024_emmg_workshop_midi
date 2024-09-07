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

#include <cstddef>
#include <cstring>

#include "debug.h"
#include "storage.h"

#include <EEPROM.h>

/* Returns true if it was already initialized */
bool Storage::init() {

  if (!legit()) {
    erase();
    return false;
  }
  return true;
}

bool Storage::legit() {
  // Check signature
  for (int i = 0; i < sizeof(signature); i++)
    if (EEPROM.read(i) != signature[i]) {
      SERIAL_PRINTLN("Signature not matching!");
      return false;
    }

  // Check Matrix Size
  if (EEPROM.read(offset_settings_size) != SETTINGS_SIZE) {
    SERIAL_PRINTLN("Settings size not matching!");
    return false;
  }

  return true;
}

void Storage::erase() {
  for (int i = 0; i < sizeof(signature); i++)
    EEPROM.write(i, signature[i]);

  for (int i = sizeof(signature); i < memory_size; i++)
    EEPROM.write(i, 0);
  EEPROM.write(offset_settings_size, SETTINGS_SIZE);  // Careful if you make the settings large than 255

  EEPROM.commit();
}

void Storage::save_settings(ControllerSettings& settings) {
  settings.saveToArray(settings_buffer);

  for (size_t i = 0; i < SETTINGS_SIZE; i++)
    EEPROM.write(offset_settings_table + i, settings_buffer[i]);

  EEPROM.commit();
}

void Storage::load_settings(ControllerSettings& settings) {
  for (int i = 0; i < SETTINGS_SIZE; i++)
    settings_buffer[i] = EEPROM.read(offset_settings_table + i);
  settings.loadFromArray(settings_buffer);
}

Storage storage;
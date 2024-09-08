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

//#include <EEPROM.h>
#include <LittleFS.h>

/* Returns true if it was already initialized */
bool Storage::init() {

  if (!LittleFS.begin()) {
    SERIAL_PRINTLN("LittleFS Mount Failed");
    return false;
  }

  if (!legit()) {
    erase();
    return false;
  }
  return true;
}

bool Storage::legit() {

  File file = LittleFS.open(STORAGE_FILE, "r");
  if (!file || file.size() != TOTAL_SIZE) {
    SERIAL_PRINTLN("Storage file not found or incorrect size!");
    return false;
  }

  char stored_signature[SIGNATURE_SIZE];
  file.read((uint8_t*)stored_signature, SIGNATURE_SIZE);
  file.close();

  if (memcmp(stored_signature, signature, SIGNATURE_SIZE) != 0) {
    SERIAL_PRINTLN("Signature not matching!");
    return false;
  }

  return true;
}

void Storage::erase() {

  File file = LittleFS.open(STORAGE_FILE, "w");
  if (file) {
    file.write((uint8_t*)signature, SIGNATURE_SIZE);

    uint8_t empty_settings[SETTINGS_SIZE] = { 0 };
    file.write(empty_settings, SETTINGS_SIZE);

    file.close();
  }
}

void Storage::save_settings(ControllerSettings& settings) {
  settings.saveToArray(settings_buffer);

  File file = LittleFS.open(STORAGE_FILE, "r+");
  if (file) {
    file.seek(SIGNATURE_SIZE, SeekSet);
    file.write(settings_buffer, SETTINGS_SIZE);
    file.close();
  }
}

void Storage::load_settings(ControllerSettings& settings) {
  File file = LittleFS.open(STORAGE_FILE, "r");
  if (file) {
    file.seek(SIGNATURE_SIZE, SeekSet);
    file.read(settings_buffer, SETTINGS_SIZE);
    file.close();
  }

  settings.loadFromArray(settings_buffer);
}


Storage storage;
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

#ifndef EMMG_2040_STORAGE_H
#define EMMG_2040_STORAGE_H

#include <cstddef>
#include <cstdint>
#include "controllersettings.h"

class Storage {
    char signature[8] = {0x42, 'E', 'M', 'M', 'G', 'V', '0', '2'};
    const uint16_t offset_settings_size = 8;
    const uint16_t offset_settings_table = 64;
    const uint16_t memory_size = 255;
    uint8_t settings_buffer[SETTINGS_SIZE]{0};
    //char buffer[8] = {0};

public:
    Storage()= default;
    /**
     * Mandatory!
     */
    bool init();

    /**
     * Erases the EEPROM
     */
    void erase();

    /**
     * Checks if the EEPROM segment contains our signature
     * @return true if yes
     */
    bool legit();

    /**
     * Save the given settings
     */
    void save_settings(ControllerSettings& settings);

    /**
     * Load into the given routing_matrix
     */
    void load_settings(ControllerSettings& settings);
};

extern Storage storage;

#endif //EMMG_2040_STORAGE_H
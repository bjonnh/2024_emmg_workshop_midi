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

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "controllersettings.h"
#include "device.h"
#include "display.h"
#include "storage.h"


enum class ControllerModeState {
  NORMAL,
  SETUP_MENU,
  CONTROLLER_SETUP,
  PAD_SETUP,
  LOAD,
  SAVE,
  PANIC
};

class SetupMenuState {
public:
  void setFromKnob(uint8_t value);
  ControllerModeState selected_state = ControllerModeState::NORMAL;
};

class ControllerSetupState {
public:
  uint8_t controller = 0;
  Controller settings;
};

class PadSetupState {
public:
  uint8_t pad = 0;
  Pad settings;
};


class ControllerMode {
public:
  ControllerMode(Device& device);
  void begin();
  void loop();
  void onKnobChange(uint8_t id, uint8_t value);
  void onTouchPadChange(uint8_t id, uint8_t value);
  void onButtonChange(uint8_t id, uint8_t value);
private:
  void update();
  void display_normal_mode();
  void display_setup_menu();
  void display_controller_setup();
  void display_pad_setup();
  Device& device;
  SetupMenuState setup_menu_state;
  ControllerSettings controller_settings;
  Storage storage;
  ControllerModeState current_state = ControllerModeState::NORMAL;
  ControllerSetupState controller_setup_state;
  PadSetupState pad_setup_state;
};


extern Controller* ControllerInstance;


#endif

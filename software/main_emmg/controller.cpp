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

#include <Arduino.h>
#include "controller.h"
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/TomThumb.h>
#include "config.h"

ControllerMode* ControllerModeInstance = nullptr;


void SetupMenuState::setFromKnob(uint8_t value) {
  if (value < 20)
    selected_state = ControllerModeState::NORMAL;
  else if (value < 40)
    selected_state = ControllerModeState::CONTROLLER_SETUP;
  else if (value < 60)
    selected_state = ControllerModeState::PAD_SETUP;
  else if (value < 80)
    selected_state = ControllerModeState::LOAD;
  else if (value < 100)
    selected_state = ControllerModeState::SAVE;
  else
    selected_state = ControllerModeState::PANIC;
}


ControllerMode::ControllerMode(Device& device)
  : device(device) {
  ControllerModeInstance = this;
}



void ControllerMode::begin() {
  if (!storage.init()) {
    SERIAL_PRINTLN("Storage not initialized");
   for (uint8_t i = 0; i < 8; i++) controller_settings.controllers[i] = Controller(20 + i, 1);
    for (uint8_t i = 0; i < 12; i++) controller_settings.pads[i] = Pad(50 + i, 1, 0, PadType::NOTE);
    SERIAL_PRINTLN("Generating default settings");
    storage.save_settings(controller_settings);
  } else {
    SERIAL_PRINTLN("Loading settings");
    storage.load_settings(controller_settings);
  }
  
  SERIAL_PRINTLN("Arming the callbacks");
  device.setKnobCallback([](uint8_t id, uint8_t value) {
    ControllerModeInstance->onKnobChange(id, value);
  });
  device.setTouchPadCallback([](uint8_t id, uint8_t value) {
    ControllerModeInstance->onTouchPadChange(id, value);
  });
  device.setButtonCallback([](uint8_t id, uint8_t value) {
    ControllerModeInstance->onButtonChange(id, value);
  });
  
  ready = true;
}

void __not_in_flash_func(ControllerMode::loop)() {
  device.poll();
  //if (clear_updated) updated = false;
  //if (updated) {
  //  updated = false;
  //  update();
  //}
}

void ControllerMode::onKnobChange(uint8_t id, uint8_t value) {
  switch (current_state) {
    case ControllerModeState::NORMAL:
      device.sendControlChange(controller_settings.controllers[id].cc, value, controller_settings.controllers[id].channel);
      break;
    case ControllerModeState::SETUP_MENU:
      if (id == 0) {
        setup_menu_state.setFromKnob(value);
      }
      break;
    case ControllerModeState::CONTROLLER_SETUP:
      if (id == 0) {
        controller_setup_state.controller = (value / 15);  // We use 15 to have 9 states so the last one is for exiting
        if (controller_setup_state.controller < 8) {
          controller_setup_state.settings.cc = controller_settings.controllers[controller_setup_state.controller].cc;
          controller_setup_state.settings.channel = controller_settings.controllers[controller_setup_state.controller].channel;
        }
      }

      if (id == 1) {
        controller_setup_state.settings.cc = value;
      }

      if (id == 2) {
        controller_setup_state.settings.channel = (value / 8) + 1;
      }
      break;
    case ControllerModeState::PAD_SETUP:
      if (id == 0) {
        pad_setup_state.pad = (value / 10);  // We use 10 to have 13 states so the last one is for exiting
        if (pad_setup_state.pad < 12) {
          pad_setup_state.settings.cc = controller_settings.pads[pad_setup_state.pad].cc;
          pad_setup_state.settings.channel = controller_settings.pads[pad_setup_state.pad].channel;
          pad_setup_state.settings.type = controller_settings.pads[pad_setup_state.pad].type;
        }
      }

      if (id == 1) {
        pad_setup_state.settings.cc = value;
      }

      if (id == 2) {
        pad_setup_state.settings.channel = (value / 8) + 1;
      }

      if (id == 3) {
        pad_setup_state.settings.type = static_cast<PadType>(value / 43);
      }
      break;
  }
  updated = true;
}

void ControllerMode::onTouchPadChange(uint8_t id, uint8_t value) {
  Pad& pad = controller_settings.pads[id];
  switch (pad.type) {
    case PadType::NOTE:
      if (value == 1) {
        device.sendNoteOn(pad.cc, 127, pad.channel);
      } else {
        device.sendNoteOff(pad.cc, 0, pad.channel);
      }
      break;
    case PadType::CC:
      device.sendControlChange(pad.cc, (value == 1) ? 127 : 0, pad.channel);
      break;
    case PadType::CC_LATCH:
      bool changed = false;
      if (value == 1 && pad.current == 0) {
        pad.current = 127;
        changed = true;
      } else if (value == 1 && pad.current == 127) {
        pad.current = 0;
        changed = true;
      }
      if (changed) device.sendControlChange(pad.cc, pad.current, pad.channel);

      break;
  }
  updated = true;
}

void ControllerMode::onButtonChange(uint8_t id, uint8_t value) {
  if (value > 0) {
    switch (current_state) {
      case ControllerModeState::NORMAL:
        current_state = ControllerModeState::SETUP_MENU;
        break;
      case ControllerModeState::SETUP_MENU:
        current_state = setup_menu_state.selected_state;
        if (current_state == ControllerModeState::CONTROLLER_SETUP) {
          if (controller_setup_state.controller < 8) {
            controller_setup_state.settings.cc = controller_settings.controllers[controller_setup_state.controller].cc;
            controller_setup_state.settings.channel = controller_settings.controllers[controller_setup_state.controller].channel;
          }
        }
        if (current_state == ControllerModeState::PANIC) {
          device.midiPanic();
          current_state = ControllerModeState::NORMAL;
        }
        if (current_state == ControllerModeState::LOAD) {
          storage.load_settings(controller_settings);
          current_state = ControllerModeState::NORMAL;
        }
        if (current_state == ControllerModeState::SAVE) {
          storage.save_settings(controller_settings);
          current_state = ControllerModeState::NORMAL;
        }
        break;
      case ControllerModeState::CONTROLLER_SETUP:
        if (controller_setup_state.controller == 8) {
          current_state = ControllerModeState::NORMAL;
        } else {
          controller_settings.controllers[controller_setup_state.controller].cc = controller_setup_state.settings.cc;
          controller_settings.controllers[controller_setup_state.controller].channel = controller_setup_state.settings.channel;
        }
        break;
      case ControllerModeState::PAD_SETUP:
        if (pad_setup_state.pad == 12) {
          current_state = ControllerModeState::NORMAL;
        } else {
          controller_settings.pads[pad_setup_state.pad].cc = pad_setup_state.settings.cc;
          controller_settings.pads[pad_setup_state.pad].channel = pad_setup_state.settings.channel;
          controller_settings.pads[pad_setup_state.pad].type = pad_setup_state.settings.type;
        }
        break;
    }
    updated = true;
  }
}

void ControllerMode::display_normal_mode() {
  device.display.adisplay->clearDisplay();
  device.display.adisplay->setFont(&FreeSans9pt7b);
  device.display.adisplay->setTextSize(1);
  device.display.adisplay->setTextColor(SSD1306_WHITE);
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t value = device.getKnobValue(i);

    device.display.adisplay->setCursor((i % 4) * 32, 20 + 24 * (i / 4));
    if (value < 100) device.display.adisplay->print(" ");
    if (value < 10) device.display.adisplay->print(" ");
    device.display.adisplay->print(value);
  }
  for (uint8_t i = 0; i < 12; i++) {
    bool touched = device.getTouchPadValue(i);
    Pad& pad = controller_settings.pads[i];
    if ((pad.type != PadType::CC_LATCH && touched) || (pad.type == PadType::CC_LATCH && pad.current == 127)) {
      device.display.adisplay->drawRect(i * 10 + 4, 52, 8, 12, SSD1306_WHITE);
    } else {

      device.display.adisplay->fillRect(i * 10 + 4, 52, 8, 12, SSD1306_WHITE);
    }
  }
  device.display.adisplay->display();
}

void ControllerMode::display_setup_menu() {
  device.display.adisplay->clearDisplay();
  device.display.adisplay->setFont(&FreeSans9pt7b);
  device.display.adisplay->setTextSize(1);
  device.display.adisplay->setTextColor(SSD1306_WHITE);
  device.display.adisplay->setCursor(0, 20);
  device.display.adisplay->print("Setup");
  device.display.adisplay->setCursor(0, 61);
  switch (setup_menu_state.selected_state) {
    case ControllerModeState::NORMAL:
      device.display.adisplay->print("< Back");
      break;
    case ControllerModeState::CONTROLLER_SETUP:
      device.display.adisplay->print("Controller");
      break;
    case ControllerModeState::PAD_SETUP:
      device.display.adisplay->print("Pad");
      break;
    case ControllerModeState::PANIC:
      device.display.adisplay->print("PANIC!");
      break;
    case ControllerModeState::LOAD:
      device.display.adisplay->print("Load");
      break;
    case ControllerModeState::SAVE:
      device.display.adisplay->print("Save");
      break;
  }
  device.display.adisplay->display();
}

void ControllerMode::display_controller_setup() {
  device.display.adisplay->clearDisplay();
  device.display.adisplay->setFont(&FreeSans9pt7b);
  device.display.adisplay->setTextSize(1);
  device.display.adisplay->setTextColor(SSD1306_WHITE);
  device.display.adisplay->setCursor(0, 61);
  if (controller_setup_state.controller < 8) {
    device.display.adisplay->print("Controller ");
    device.display.adisplay->print(controller_setup_state.controller + 1);
    device.display.adisplay->setCursor(0, 20);
    device.display.adisplay->print("CC         -> ");
    device.display.adisplay->setCursor(40, 20);
    device.display.adisplay->print(controller_settings.controllers[controller_setup_state.controller].cc);
    device.display.adisplay->setCursor(90, 20);
    device.display.adisplay->print(controller_setup_state.settings.cc);
    device.display.adisplay->setCursor(0, 40);
    device.display.adisplay->print("CH         -> ");
    device.display.adisplay->setCursor(40, 40);
    device.display.adisplay->print(controller_settings.controllers[controller_setup_state.controller].channel);
    device.display.adisplay->setCursor(90, 40);
    device.display.adisplay->print(controller_setup_state.settings.channel);
  } else {
    device.display.adisplay->print("< Back");
  }
  device.display.adisplay->display();
}

void ControllerMode::display_pad_setup() {
  device.display.adisplay->clearDisplay();
  device.display.adisplay->setFont(&FreeSans9pt7b);
  device.display.adisplay->setTextSize(1);
  device.display.adisplay->setTextColor(SSD1306_WHITE);
  device.display.adisplay->setCursor(0, 61);
  if (pad_setup_state.pad < 12) {
    device.display.adisplay->print("Pad ");
    device.display.adisplay->print(pad_setup_state.pad + 1);
    device.display.adisplay->setCursor(70, 14);
    device.display.adisplay->print("->");

    device.display.adisplay->setCursor(0, 14);
    device.display.adisplay->print(to_string(controller_settings.pads[pad_setup_state.pad].type));

    device.display.adisplay->setCursor(90, 14);
    device.display.adisplay->print(to_string(pad_setup_state.settings.type));



    device.display.adisplay->setCursor(70, 30);
    device.display.adisplay->print("->");

    device.display.adisplay->setCursor(40, 30);
    device.display.adisplay->print(controller_settings.pads[pad_setup_state.pad].cc);
    device.display.adisplay->setCursor(90, 30);
    device.display.adisplay->print(pad_setup_state.settings.cc);
    device.display.adisplay->setCursor(0, 46);
    device.display.adisplay->print("CH");
    device.display.adisplay->setCursor(70, 46);
    device.display.adisplay->print("->");

    device.display.adisplay->setCursor(40, 46);
    device.display.adisplay->print(controller_settings.pads[pad_setup_state.pad].channel);
    device.display.adisplay->setCursor(90, 46);
    device.display.adisplay->print(pad_setup_state.settings.channel);
  } else {
    device.display.adisplay->print("< Back");
  }
  device.display.adisplay->display();
}

void ControllerMode::update() {
  if (!ready) return;
  //if (!updated || last_forced_update < 10) {
  //  last_forced_update++;
  //  return;
  //}
  //last_forced_update = 0;
  switch (current_state) {
    case ControllerModeState::NORMAL:
      display_normal_mode();
      break;
    case ControllerModeState::SETUP_MENU:
      display_setup_menu();
      break;
    case ControllerModeState::CONTROLLER_SETUP:
      display_controller_setup();
      break;
    case ControllerModeState::PAD_SETUP:
      display_pad_setup();
      break;
  }
  //clear_updated = true;
}

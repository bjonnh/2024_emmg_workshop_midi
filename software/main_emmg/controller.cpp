#include <Arduino.h>
#include "controller.h"
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/TomThumb.h>

ControllerMode* ControllerModeInstance = nullptr;


void SetupMenuState::setFromKnob(uint8_t value) {
  if (value < 40)
    selected_state = ControllerModeState::NORMAL;
  else if (value < 80)
    selected_state = ControllerModeState::CONTROLLER_SETUP;
  else
    selected_state = ControllerModeState::PAD_SETUP;
}

Controller::Controller(uint8_t cc, uint8_t channel)
  : cc(cc), channel(channel){};

Pad::Pad(uint8_t note, uint8_t channel)
  : note(note), channel(channel){};

ControllerMode::ControllerMode(Device& device)
  : device(device) {
  ControllerModeInstance = this;
  for (uint8_t i = 0; i < 8; i++) controllers[i] = Controller(20 + i, 1);
  for (uint8_t i = 0; i < 12; i++) pads[i] = Pad(50 + i, 1);

  device.setKnobCallback([](uint8_t id, uint8_t value) {
    ControllerModeInstance->onKnobChange(id, value);
  });
  device.setTouchPadCallback([](uint8_t id, uint8_t value) {
    ControllerModeInstance->onTouchPadChange(id, value);
  });
  device.setButtonCallback([](uint8_t id, uint8_t value) {
    ControllerModeInstance->onButtonChange(id, value);
  });
}

void ControllerMode::loop() {
  device.poll();
}

void ControllerMode::onKnobChange(uint8_t id, uint8_t value) {
  switch (current_state) {
    case ControllerModeState::NORMAL:
      device.sendControlChange(controllers[id].cc, value, controllers[id].channel);
      update();
      break;
    case ControllerModeState::SETUP_MENU:
      if (id == 0) {
        setup_menu_state.setFromKnob(value);
        update();
      }
      break;
    case ControllerModeState::CONTROLLER_SETUP:
      if (id == 0) {
        controller_setup_state.controller = (value / 15);  // We use 15 to have 9 states so the last one is for exiting
        if (controller_setup_state.controller < 8) {
          controller_setup_state.cc = controllers[controller_setup_state.controller].cc;
          controller_setup_state.channel = controllers[controller_setup_state.controller].channel;
        }
        update();
      }

      if (id == 1) {
        controller_setup_state.cc = value;

        update();
      }

      if (id == 2) {
        controller_setup_state.channel = (value / 8) + 1;
        update();
      }
      break;
    case ControllerModeState::PAD_SETUP:
      if (id == 0) {
        pad_setup_state.pad = (value / 10);  // We use 10 to have 13 states so the last one is for exiting
        if (pad_setup_state.pad < 12) {
          pad_setup_state.note = pads[pad_setup_state.pad].note;
          pad_setup_state.channel = pads[pad_setup_state.pad].channel;
        }
        update();
      }

      if (id == 1) {
        pad_setup_state.note = value;

        update();
      }

      if (id == 2) {
        pad_setup_state.channel = (value / 8) + 1;
        update();
      }
      break;
  }
}

void ControllerMode::onTouchPadChange(uint8_t id, uint8_t value) {
  if (value == 1) {
    device.sendNoteOn(pads[id].note, 127, pads[id].channel);
  } else {
    device.sendNoteOff(pads[id].note, 0, pads[id].channel);
  }
  update();
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
            controller_setup_state.cc = controllers[controller_setup_state.controller].cc;
            controller_setup_state.channel = controllers[controller_setup_state.controller].channel;
          }
        }
        break;
      case ControllerModeState::CONTROLLER_SETUP:
        if (controller_setup_state.controller == 8) {
          current_state = ControllerModeState::NORMAL;
        } else {
          controllers[controller_setup_state.controller].cc = controller_setup_state.cc;
          controllers[controller_setup_state.controller].channel = controller_setup_state.channel;
        }
        break;
      case ControllerModeState::PAD_SETUP:
        if (pad_setup_state.pad == 12) {
          current_state = ControllerModeState::NORMAL;
        } else {
          pads[pad_setup_state.pad].note = pad_setup_state.note;
          pads[pad_setup_state.pad].channel = pad_setup_state.channel;
        }
        break;
    }
    update();
  }
}

void ControllerMode::display_normal_mode() {
  device.display.adisplay.clearDisplay();
  device.display.adisplay.setFont(&FreeSans9pt7b);
  device.display.adisplay.setTextSize(1);
  device.display.adisplay.setTextColor(SSD1306_WHITE);
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t value = device.getKnobValue(i);

    device.display.adisplay.setCursor((i % 4) * 32, 20 + 24 * (i / 4));
    if (value < 100) device.display.adisplay.print(" ");
    if (value < 10) device.display.adisplay.print(" ");
    device.display.adisplay.print(value);
  }
  for (uint8_t i = 0; i < 12; i++) {
    bool touched = device.getTouchPadValue(i);
    if (touched) {
      device.display.adisplay.drawRect(i * 10 + 4, 52, 8, 12, SSD1306_WHITE);
    } else {
      device.display.adisplay.fillRect(i * 10 + 4, 52, 8, 12, SSD1306_WHITE);
    }
  }
  device.display.adisplay.display();
}

void ControllerMode::display_setup_menu() {
  device.display.adisplay.clearDisplay();
  device.display.adisplay.setFont(&FreeSans9pt7b);
  device.display.adisplay.setTextSize(1);
  device.display.adisplay.setTextColor(SSD1306_WHITE);
  device.display.adisplay.setCursor(0, 20);
  device.display.adisplay.print("Setup");
  device.display.adisplay.setCursor(0, 61);
  switch (setup_menu_state.selected_state) {
    case ControllerModeState::NORMAL:
      device.display.adisplay.print("< Back");
      break;
    case ControllerModeState::CONTROLLER_SETUP:
      device.display.adisplay.print("Controller");
      break;
    case ControllerModeState::PAD_SETUP:
      device.display.adisplay.print("Pad");
      break;
  }
  device.display.adisplay.display();
}

void ControllerMode::display_controller_setup() {
  device.display.adisplay.clearDisplay();
  device.display.adisplay.setFont(&FreeSans9pt7b);
  device.display.adisplay.setTextSize(1);
  device.display.adisplay.setTextColor(SSD1306_WHITE);
  device.display.adisplay.setCursor(0, 61);
  if (controller_setup_state.controller < 8) {
    device.display.adisplay.print("Controller ");
    device.display.adisplay.print(controller_setup_state.controller + 1);
    device.display.adisplay.setCursor(0, 20);
    device.display.adisplay.print("CC         -> ");
    device.display.adisplay.setCursor(40, 20);
    device.display.adisplay.print(controllers[controller_setup_state.controller].cc);
    device.display.adisplay.setCursor(90, 20);
    device.display.adisplay.print(controller_setup_state.cc);
    device.display.adisplay.setCursor(0, 40);
    device.display.adisplay.print("CH         -> ");
    device.display.adisplay.setCursor(40, 40);
    device.display.adisplay.print(controllers[controller_setup_state.controller].channel);
    device.display.adisplay.setCursor(90, 40);
    device.display.adisplay.print(controller_setup_state.channel);
  } else {
    device.display.adisplay.print("< Back");
  }
  device.display.adisplay.display();
}

void ControllerMode::display_pad_setup() {
  device.display.adisplay.clearDisplay();
  device.display.adisplay.setFont(&FreeSans9pt7b);
  device.display.adisplay.setTextSize(1);
  device.display.adisplay.setTextColor(SSD1306_WHITE);
  device.display.adisplay.setCursor(0, 61);
  if (pad_setup_state.pad < 12) {
    device.display.adisplay.print("Pad ");
    device.display.adisplay.print(pad_setup_state.pad + 1);
    device.display.adisplay.setCursor(0, 20);
    device.display.adisplay.print("Note       -> ");
    device.display.adisplay.setCursor(40, 20);
    device.display.adisplay.print(pads[pad_setup_state.pad].note);
    device.display.adisplay.setCursor(90, 20);
    device.display.adisplay.print(pad_setup_state.note);
    device.display.adisplay.setCursor(0, 40);
    device.display.adisplay.print("CH         -> ");
    device.display.adisplay.setCursor(40, 40);
    device.display.adisplay.print(pads[pad_setup_state.pad].channel);
    device.display.adisplay.setCursor(90, 40);
    device.display.adisplay.print(pad_setup_state.channel);
  } else {
    device.display.adisplay.print("< Back");
  }
  device.display.adisplay.display();
}

void ControllerMode::update() {
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
}

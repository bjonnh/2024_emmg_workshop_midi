#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "device.h"
#include "display.h"
#include <EEPROM.h>



enum class ControllerModeState {
  NORMAL,
  SETUP_MENU,
  CONTROLLER_SETUP,
  PAD_SETUP, 
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
    uint8_t cc = 0;
    uint8_t channel = 0;
};

class PadSetupState {
  public:
    uint8_t pad = 0;
    uint8_t note = 0;
    uint8_t channel = 0;
};

class Controller {
  public:
    Controller() {};
    Controller(uint8_t cc, uint8_t channel);
    uint8_t cc = 0;
    uint8_t channel = 1;
};

class Pad {
  public:
    Pad() {};
    Pad(uint8_t note, uint8_t channel);
    uint8_t note = 0;
    uint8_t channel = 1;
};

class ControllerMode {
  public:
    ControllerMode(Device& device);
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
    ControllerSetupState controller_setup_state;
    PadSetupState pad_setup_state;
    ControllerModeState current_state = ControllerModeState::NORMAL;
    Controller controllers[8];
    Pad pads[12];
};




extern Controller* ControllerInstance;


#endif

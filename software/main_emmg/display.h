#ifndef DISPLAY_H
#define DISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "debug.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C
#define SCREEN_SDA 18
#define SCREEN_SCL 19

class Display {
  public:
    Display();
    void begin();
    void test();

    Adafruit_SSD1306 adisplay;
};

#endif // DISPLAY_H

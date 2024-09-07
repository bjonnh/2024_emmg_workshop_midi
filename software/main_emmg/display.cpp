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


#include "display.h"


Display::Display() : adisplay(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, -1) {};

void Display::begin() {
  delay(100);
  SERIAL_PRINTLN("Booting display");
  Wire1.setSDA(SCREEN_SDA);
  Wire1.setSCL(SCREEN_SCL);
  Wire1.setClock(1000000);
  Wire1.begin();

  if (!adisplay.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    SERIAL_PRINTLN(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  delay(10);
  adisplay.setRotation(2);
  adisplay.clearDisplay();
  adisplay.display();
  delay(100);
}

void Display::test() {
  adisplay.setTextSize(1);
  adisplay.setTextColor(SSD1306_WHITE);
  adisplay.setCursor(0, 0);
  adisplay.println(F("Hello, world!"));
  adisplay.display();
}
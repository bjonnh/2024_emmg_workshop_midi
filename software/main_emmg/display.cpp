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

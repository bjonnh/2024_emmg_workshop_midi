#include <Wire.h> //include Wire.h library
#include "display.h"
#include "device.h"
 
Display display;
Device device;


void setup()
{
  Serial.begin(9600); // The baudrate of Serial monitor is set in 9600
  while (!Serial); // Waiting for Serial Monitor
        
  Serial.println("\nI2C Scanner");
  
  display.begin();  
  display.test();

  device.begin();
  
}

void loop()
{
  device.poll();

  // Rewrite screen
  if (device.isUpdated()) {
    display.adisplay.clearDisplay();
    for (uint8_t i=0; i<8 ; i++) {
      uint8_t value = device.getKnobValue(i);
      display.adisplay.setTextSize(1);
      display.adisplay.setTextColor(SSD1306_WHITE);
      display.adisplay.setCursor((i%4)*32, 32*(i/4));
      if (value < 100) display.adisplay.print(" ");
      if (value < 10) display.adisplay.print(" ");
      display.adisplay.print(value);
      
    }
    for (uint8_t i=0; i<12 ; i++) {
      bool touched = device.getTouchPadValue(i);
      if (touched) {
        display.adisplay.drawRect(i*10+4,44,8,4, SSD1306_WHITE);
      } else {
        display.adisplay.fillRect(i*10+4,44,8,4, SSD1306_WHITE);
      }
    }
    display.adisplay.display();
  }
  //
   
  delay(10);
}

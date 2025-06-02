/**
 * display.h: header files for display.cpp
*/

#ifndef DISPLAY_H
#define DISPLAY_H


// Display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED
#define OLED_RESET -1
Adafruit_SSD1306 display(128, 32, &Wire, OLED_RESET);

class Display {
public:
  static void startScreen();
}

#endif  // DISPLAY_H
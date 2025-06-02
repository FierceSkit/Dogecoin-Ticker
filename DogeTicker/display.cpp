/**
 * display.cpp: handles display support
*/

#include "display.h"
#include <Wire.h>
Adafruit_SSD1306* display;

void Display::startScreen() {
  display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

  // initialize with the I2C addr 0x3C for 128x32 OLED
  Serial.println("Initializing Display ...");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();  // Clear the display buffer
  display.display();
  delay(25);
  display.clearDisplay();
  delay(25);
  display.setTextColor(WHITE);
  delay(100);
}
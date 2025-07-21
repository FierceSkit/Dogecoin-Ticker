#ifndef CURRENCY_SYMBOLS_H
#define CURRENCY_SYMBOLS_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "bitmaps.h"

// Currency Symbols (UTF-8)
const char SYMBOL_USD[] = "$";      // US Dollar
const char SYMBOL_EUR[] = "\xE2\x82\xAC";  // Euro (€)
const char SYMBOL_GBP[] = "\xC2\xA3";      // British Pound (£)
const char SYMBOL_RUB[] = "\xE2\x82\xBD";  // Russian Ruble (₽)
const char SYMBOL_SGD[] = "S$";     // Singapore Dollar

// Function to display the appropriate coin splash screen
void showCoinSplash(Adafruit_SSD1306& display, String coin) {
  display.clearDisplay();
  
  // Select appropriate logo based on coin
  if (coin == "DOGE") {
    display.drawBitmap(0, 0, DOGE_LOGO, 128, 32, 1);
  } else if (coin == "BTC") {
    display.drawBitmap(0, 0, BTC_LOGO, 128, 32, 1);
  } else if (coin == "LTC") {
    display.drawBitmap(0, 0, LTC_LOGO, 128, 32, 1);
  } else {
    // If no specific logo, show DOGE as default
    display.drawBitmap(0, 0, DOGE_LOGO, 128, 32, 1);
  }
  
  display.display();
  delay(2000);  // Show splash for 2 seconds
}

#endif // CURRENCY_SYMBOLS_H 
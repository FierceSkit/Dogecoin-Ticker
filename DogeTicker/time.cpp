/**
 * time.cpp: handles the time-init of the Ticker
*/

#include "time.h"

/** developer note:
 *
 * the time frame is defined here.
 * this is a raw frame(layer 2)
 * man i hate timekeeping
 *
*/

 // Set time via NTP, as required for x.509 validation
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");{

  Serial.print("Waiting for NTP time sync: ");

  clearDisplay();
  display.setCursor(1, 0);
  display.print("Syncing Time");
  updateDisplay();

  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    display.print(".");
    updateDisplay();
    now = time(nullptr);

    updateDisplay();
  }

  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);

  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

  // Show time on display
  clearDisplay();
  display.setCursor(1, 0);
  display.print("Current Time");
  display.setCursor(1, 10);
  display.println(asctime(&timeinfo));
  updateDisplay();

  delay(5000);
}
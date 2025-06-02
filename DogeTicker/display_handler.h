#ifndef DISPLAY_HANDLER_H
#define DISPLAY_HANDLER_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include "bitmaps.h"
#include "currency_symbols.h"

class DisplayHandler {
private:
    Adafruit_SSD1306* display;
    bool displayInitialized;

public:
    DisplayHandler(Adafruit_SSD1306* disp) : display(disp), displayInitialized(false) {}

    bool begin() {
        displayInitialized = display->begin(SSD1306_SWITCHCAPVCC, 0x3C);
        if (!displayInitialized) {
            return false;
        }
        
        display->clearDisplay();
        display->setTextColor(SSD1306_WHITE);
        display->setTextSize(1);
        display->display();
        return true;
    }

    void showWiFiConnecting(int attempt, int maxAttempts) {
        if (!displayInitialized) return;
        
        display->clearDisplay();
        display->setTextSize(1);
        display->setTextColor(SSD1306_WHITE);
        display->setCursor(10, 0);
        display->println("Connecting to WiFi");
        display->setCursor(0, 20);
        display->print("Attempt: ");
        display->print(attempt);
        display->print("/");
        display->print(maxAttempts);
        display->display();
    }

    void showWiFiError(const char* ssid) {
        if (!displayInitialized) return;
        
        display->clearDisplay();
        display->setCursor(0, 0);
        display->println("WiFi Failed!");
        display->setCursor(0, 10);
        display->println("Please check:");
        display->setCursor(0, 20);
        display->println("1. SSID: " + String(ssid));
        display->setCursor(0, 30);
        display->println("2. Password in code");
        display->display();
    }

    void showWiFiSuccess(IPAddress ip) {
        if (!displayInitialized) return;
        
        display->clearDisplay();
        display->setCursor(11, 0);
        display->println("Connected to WiFi");
        display->setCursor(30, 13);
        display->print("IP Address");
        display->setCursor(23, 23);
        display->print(ip);
        display->display();
    }

    void showCoinSplash(const String& coin) {
        display->clearDisplay();
        display->setTextSize(1);
        display->setTextColor(WHITE);
        
        // Select appropriate logo
        const unsigned char* logo;
        if (coin == "BTC") {
            logo = BTC_LOGO;
        } else if (coin == "DOGE") {
            logo = DOGE_LOGO;
        } else if (coin == "LTC") {
            logo = LTC_LOGO;
        }
        
        display->drawBitmap(0, 0, logo, 128, 32, WHITE);
        display->display();
    }

    void updatePrice(const String& base, const String& target, const String& price, float change) {
        display->clearDisplay();

        // Set ticker
        display->setFont();
        display->setTextColor(BLACK, WHITE);
        display->setCursor(1, 0);
        display->print(base);
        display->print(" => ");
        display->print(target);

        // Set the current price
        display->setCursor(1, 16);
        display->setTextColor(WHITE);
        display->setFont(&FreeSansBold9pt7b);
        
        // Choose and display currency symbol
        const char* currencySymbol = getCurrencySymbol(target);
        display->setTextSize(1);  // Reset text size for symbol
        display->print(currencySymbol);
        display->print(" ");
        
        // For JPY, show without decimal places
        if (target == "JPY") {
            display->print(String((int)price.toFloat()));
        } else {
            display->print(price.substring(0, 11));
        }
        display->setFont();

        // Set the 24-hour change
        display->setCursor(1, 25);
        display->setTextSize(1);
        display->setTextColor(WHITE);
        float changePercent = change * 100.0;
        display->print("Change: ");
        display->print(changePercent, 2);
        display->println(" %");

        display->display();
    }

    void showError(const String& type, const String& error) {
        display->clearDisplay();
        
        // Set Title
        display->setTextColor(BLACK, WHITE);
        display->setCursor(1, 0);
        display->print(type);

        // Show Error - shortened message
        display->setCursor(1, 16);
        display->setTextSize(1);
        display->setTextColor(WHITE);
        
        // Shorten "Price pair not available" to "Pair N/A"
        if (error == "Price pair not available") {
            display->print("Pair N/A");
        } else {
            display->print(error);
        }

        display->display();  // Ensure full display update
    }

    void showLoading(const String& title, const String& message) {
        display->clearDisplay();
        display->setTextSize(1);
        display->setTextColor(SSD1306_WHITE);
        
        // Draw loading animation
        static const uint8_t LOADING_X = 118;  // Right side of screen
        static const uint8_t LOADING_Y = 24;   // Bottom area
        static uint8_t loadingFrame = 2;
        const char* frames[] = {"|", "/", "-", "\\"};
        
        // Draw title at top
        display->setCursor(0, 0);
        display->println(title);
        
        // Draw message in middle
        display->setCursor(0, 12);
        display->println(message);
        
        // Draw animation frame
        display->setCursor(LOADING_X, LOADING_Y);
        display->print(frames[loadingFrame]);
        loadingFrame = (loadingFrame + 1) % 4;
        
        display->display();
    }

private:
    const char* getCurrencySymbol(const String& currency) {
        if (currency == "USD") return SYMBOL_USD;
        if (currency == "EUR") return SYMBOL_EUR;
        if (currency == "GBP") return SYMBOL_GBP;
        if (currency == "RUB") return SYMBOL_RUB;
        if (currency == "SGD") return SYMBOL_SGD;
        return SYMBOL_USD; // Default to USD
    }
};

#endif // DISPLAY_HANDLER_H 
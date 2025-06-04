/*
  Copyright © 2022 NexGen Digital Solutions, LLC

  This product incorporates or references certain Open Source software which requires attribution for its use.
  The license text and in some cases links to web sites hosting the license text are provided below.
  In some cases, we provide reference links to the source code used. Links to external sites are not
  under the control of NexGen Digital Solutions, LLC, who is the developer of the Crypto Ticker software
  that incorporates the following open source software, NexGen Digital Solutions does not guarantee the
  accessibility or content of those sites.

  LINK TO FULL DISCLOSURE: https://github.com/NexGen-Digital-Solutions/NexGen-Crypto-Ticker/blob/main/LICENSE

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software
  and associated documentation files (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions: The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
  THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "LittleFS.h"

#include "display_handler.h"
#include "api_handler.h"
#include "led_handler.h"
#include "button_handler.h"
#include "websocket_handler.h"
#include "wifi_handler.h"

// Network Credentials
// These are now handled by the WiFi handler and stored in LittleFS

// Pin Definitions
#define ONBOARDLED 2
#define posLed 14
#define negLed 12
#define infoLed 13
#define OLED_RESET -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR 0x3C

// Available cryptocurrencies
const int NUM_CRYPTOCURRENCIES = 3;
const String CRYPTOCURRENCIES[NUM_CRYPTOCURRENCIES] = {"BTC", "DOGE", "LTC"};
int currentCryptoIndex = 0;

// Available fiat currencies
const int NUM_FIAT_CURRENCIES = 5;
const String FIAT_CURRENCIES[NUM_FIAT_CURRENCIES] = {"USD", "EUR", "GBP", "RUB", "SGD"};
int currentFiatIndex = 0;

// Global variables
String currentCrypto = "BTC";
String currentCurrency = "USD";
unsigned long previousFetch = 0;
const long fetchInterval = 30000;
bool isPreviewMode = false;
unsigned long previewStartTime = 0;
const unsigned long PREVIEW_DURATION = 2000;

// Create display instance
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Create handlers
DisplayHandler displayHandler(&display);
LedHandler ledHandler(ONBOARDLED, posLed, negLed, infoLed);
ApiHandler apiHandler(&displayHandler);
ButtonHandler buttonHandler;
WebSocketHandler webSocketHandler;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Initialize WiFi handler with server
WiFiHandler wifiHandler(&displayHandler, &ledHandler, &server);

// Button callbacks
void onShortPress() {
    // Calculate next crypto index but don't change current yet
    int nextCryptoIndex = (currentCryptoIndex + 1) % NUM_CRYPTOCURRENCIES;
    String nextCrypto = CRYPTOCURRENCIES[nextCryptoIndex];
    
    // Show preview
    displayHandler.showCoinSplash(nextCrypto);
    
    // Set preview mode
    isPreviewMode = true;
    previewStartTime = millis();
    
    // Visual feedback
    ledHandler.flashInfo(1);
}

void onLongPress() {
    // Only allow currency changes for BTC
    if (currentCrypto != "BTC") {
        // Visual feedback for denied action
        ledHandler.flashInfo(3); // Flash 3 times to indicate invalid action
        return;
      }

    // Cycle to next fiat currency
    currentFiatIndex = (currentFiatIndex + 1) % NUM_FIAT_CURRENCIES;
    currentCurrency = FIAT_CURRENCIES[currentFiatIndex];
    
    // Force immediate API update
        previousFetch = 0;

    // Notify web clients
    String states = webSocketHandler.getCurrentStates();
    webSocketHandler.notifyClients(states);
    
    // Visual feedback
    ledHandler.flashPos(1);
}

// API callback
void onPriceUpdate(const String& price, float change) {
    displayHandler.updatePrice(currentCrypto, currentCurrency, price, change);
    ledHandler.updateLed(change);
}

void setup() {
    Serial.begin(115200);
    delay(100); // Give serial a moment to start
    
    Serial.println("Starting setup...");
    
    // Initialize I2C
    Wire.begin();
    Wire.setClock(400000); // Set I2C clock to 400kHz
    
    Serial.println("I2C initialized");
    
    // Initialize display
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
        Serial.println("SSD1306 allocation failed");
        for(;;); // Don't proceed, loop forever
    }
    
    Serial.println("Display initialized");
    
    // Show startup screen
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0,0);
    display.println("NexGen Crypto Ticker");
    display.display();
    delay(2000);
    
    // Initialize components
    displayHandler.begin();
    ledHandler.begin();
    
    // Initialize button and set callbacks
    buttonHandler.begin();
    buttonHandler.setCallbacks(onShortPress, onLongPress);
    
    // Set API callback
    apiHandler.setUpdateCallback(onPriceUpdate);
    
    // Initialize filesystem
    if (!LittleFS.begin()) {
        Serial.println("An error has occurred while mounting LittleFS");
        return;
    }
    
    // Initialize WiFi and OTA
    if (!wifiHandler.begin()) {
        // If WiFi connection fails, the handler will start AP mode
        // and the captive portal will be available
        Serial.println("WiFi connection failed, AP mode started");
    }
    wifiHandler.setupOTA();
    
    // Initialize WebSocket
    webSocketHandler.begin(&server, currentCrypto, currentCurrency, previousFetch);

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html", false);
    });

    // Route for settings page
    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html", false);
    });

    // Route for WiFi configuration
    server.on("/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/wifi_config.html", "text/html", false);
    });

    server.serveStatic("/", LittleFS, "/");
    server.begin();
    
    Serial.println("Setup complete");
}

void loop() {
    unsigned long currentTime = millis();
    
    // Handle WiFi connection and captive portal
    wifiHandler.handle();
    
    // Check if preview mode should end
    if (isPreviewMode && (currentTime - previewStartTime >= PREVIEW_DURATION)) {
        isPreviewMode = false;
        currentCryptoIndex = (currentCryptoIndex + 1) % NUM_CRYPTOCURRENCIES;
        currentCrypto = CRYPTOCURRENCIES[currentCryptoIndex];
        previousFetch = 0;  // Force immediate API update
    }
    
    // Only fetch API if not in preview mode and WiFi is connected
    if (!isPreviewMode && WiFi.status() == WL_CONNECTED && (currentTime - previousFetch >= fetchInterval)) {
        previousFetch = currentTime;
        apiHandler.fetchPrice(currentCrypto, currentCurrency);
    }
    
    ArduinoOTA.handle();
    webSocketHandler.cleanupClients();
    buttonHandler.handle();
}

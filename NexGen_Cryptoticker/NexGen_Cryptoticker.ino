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
#include <WiFiClientSecure.h>

// Display
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Fonts
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>

// JSON Includes
#include <Arduino_JSON.h>
#include <ArduinoJson.h>

// OTA & WebServer Includes
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "LittleFS.h"
#include <ElegantOTA.h>


// ===================================== //
// ===== Definitions and Variables ===== //
// ===================================== //
// Code Version Info
String codeVersion = "1.0.0";

// Network Credentials
#define ssid "YOUR_SSID"  // Your WiFi Network Name (Case Sensitive)
#define password "YOUR_PASSWD"  // Your WiFi Network Password (Case Sensitive)

// API Variables
String currentCrypto = "BTC"; // Stores default/currently selected cryptocoin
String currentCurrency = "USD"; // Stores default/currently selected currency

const long fetchInterval = 30000; // Time between API Fetch :: 30 seconds

// API Definitions
#define API_HOST ("api.gemini.com") // API Endpoint Host
#define API_URL ("/v1/pricefeed/" + coin + target) // API Endpoint Target

// API SHA1 HASH
// SHA1 fingerprint of the certificate for API Endpoint Host (host)
const char fingerprint[] PROGMEM = "ED 60 FF 86 46 D7 B1 3D 4D 73 9B 3F 32 9A EE F8 86 77 C3 F6";

// Port used to connect to API Endpoint Host.
// Since we're using SSL, we'll use port 443.
const int httpsPort = 443;

// OLED
#define OLED_RESET -1
Adafruit_SSD1306 display(128, 32, &Wire, OLED_RESET);

// LED Definitions
#define ONBOARDLED 2 // Built in LED on ESP-12/ESP-07
#define posLed 14    // Green LED
#define negLed 12    // Red LED
#define infoLed 13   // Blue LED

// These variables are used to store LED status
// False: Off / True: On
bool onboardLedStatus;
bool posLedStatus;
bool negLedStatus;
bool infoLedStatus;

// Websocket Variables
const char* sender; // Variable to store who is sending websocket messages

// Miscellaneous
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1
String values;
#define NUM_STATES  1
String stateVars[NUM_STATES] = {values}; //
unsigned long previousFetch = 0; // Stores the last time API was fetched

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");



void initializeDisplay() {
  Serial.println("Initializing Display ...");

  // initialize with the I2C addr 0x3C for 128x32 OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  display.clearDisplay();  // Clear the display buffer

  // Display Text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(1, 0);
  display.print("Initializing");
  display.setCursor(1, 10);
  display.print("Version: " + codeVersion);
  display.display();

}

// Initialize LittleFS
void initLittleFS() {
  Serial.println("Initializing Webserver");

  display.clearDisplay();  // Clear the display buffer

  // Display Text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 15);
  display.print("Starting Webserver");
  display.display();

  if (!LittleFS.begin()) {

    Serial.println("An error has occurred while mounting LittleFS");

    display.setCursor(1, 10);
    display.print("Failed to start Webserver");
    display.display();
  }

  Serial.println("LittleFS mounted successfully");

  display.setCursor(20, 25);
  display.print("Success!");
  display.display();
}

// Initialize WiFi
void initWiFi() {

  Serial.print("Initializing WiFi\n");

  display.clearDisplay();  // Clear the display buffer

  // Display Text
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(10, 0);
  display.println("Connecting to WiFi");
  display.display();

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');

    display.setCursor(0, 20);
    display.print("...");
    display.display();

    delay(1000);
  }

  Serial.println(WiFi.localIP());

  display.clearDisplay();  // Clear the display buffer
  display.setCursor(11, 0);
  display.println("Connected to WiFi");
  display.setCursor(30, 13);
  display.print("IP Address");
  display.setCursor(23, 23);
  display.print(WiFi.localIP());
  display.display();

  delay(5000);

}

String getCurrentStates() {

  JSONVar jsonData;

  for (int i = 0; i < NUM_STATES; i++) {
    jsonData["states"][i]["sender"] = "esp8266";
    jsonData["states"][i]["currentCurrency"] = currentCurrency;
    jsonData["states"][i]["currentCrypto"] = currentCrypto;
  }

  String jsonString = JSON.stringify(jsonData);

  //Serial.println("\ngetCurrentStates() JSON:\n" + jsonString);

  return jsonString;

}

void notifyClients(String state) {
  ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    //data[len] = 0;

    Serial.print("Received Websocket Message: ");
    Serial.print((char*)data); Serial.print("\n");

    if (strcmp((char*)data, "getCurrentStates") == 0) {
      ws.textAll(getCurrentStates()); //  Send data back to all clients
    } else {

      StaticJsonDocument<192> doc;

      DeserializationError error = deserializeJson(doc, (char*)data);

      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
      }

      JsonObject states_0 = doc["states"][0];
      auto states_sender = states_0["sender"].as<const char*>(); // "esp8266"
      String states_currentCurrency = states_0["currentCurrency"]; // "USD"
      String states_currentCrypto = states_0["currentCrypto"]; // "DOGE"

      sender = states_sender;

      //Serial.println(sender);
      //Serial.println(currentCrypto);
      //Serial.println(currentCurrency);

      if (strcmp((char*)sender, "client") == 0) {

        Serial.println("Received message from client.");

        currentCrypto = states_currentCrypto;
        currentCurrency = states_currentCurrency;

        previousFetch = 0;

        ws.textAll(getCurrentStates()); //  Send data back to all clients


      } else if (strcmp((char*)sender, "esp8266") == 0) {

        Serial.println("Sending message to client.");
        ws.textAll(getCurrentStates()); //  Send data back to all clients

      }
    }
  }

}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// Function to convert String to char*
//https://stackoverflow.com/questions/51531033/string-to-char-in-arduino
char* string2char(String ipString) { // make it to return pointer not a single char
  char* opChar = new char[ipString.length() + 1]; // local array should not be returned as it will be destroyed outside of the scope of this function. So create it with new operator.
  memset(opChar, 0, ipString.length() + 1);

  for (int i = 0; i < ipString.length(); i++)
    opChar[i] = ipString.charAt(i);
  return opChar; //Add this return statement.
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);

  initializeDisplay(); // Initialize the display
  setupLeds();

  initLittleFS();
  initWiFi();
  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(LittleFS, "/index.html", "text/html", false);
  });

  server.serveStatic("/", LittleFS, "/");

  // Start ElegantOTA
  ElegantOTA.begin(&server);

  // Start server
  server.begin();
}

void loop() {

  unsigned long currentFetch = millis();

  if (currentFetch - previousFetch >= fetchInterval) {

    previousFetch = currentFetch; // Store the time

    fetchApi(currentCrypto, currentCurrency);
  }

  ws.cleanupClients();

}

// Fetch data from the API
void fetchApi(String coin, String target) {
  Serial.println("Fetching '" + coin + "/" + target + "' data from API.");

  WiFiClientSecure client;              // Connect to our API URL
  client.setFingerprint(fingerprint);   // Set the Fingerprint for SSL

  // If we can't connect...
  if (!client.connect(API_HOST, httpsPort)) {
    Serial.println("Can't connect to: ");
    Serial.print(API_HOST);

    String apiError = "Can't connect to API!";

    // Show error on display
    displayError("API Error", apiError);

    return;
  }

  // Otherwise, set headers
  String request = ("GET " +  API_URL + " HTTP/1.1\r\n" +
                    "Host: " + API_HOST + "\r\n" +
                    "User-Agent: ESP8266\r\n" +
                    "Accept: */*\r\n" +
                    "Connection: close\r\n\r\n");

  client.print(request);

  // While we are connected, read the data
  while (client.connected()) {

    String line = client.readStringUntil('\n');
    if (line == "\r") {
      //Serial.println("==========\nHeaders Received\n==========\n");
      break;
    }
  }

  // Load our JSON into data variable
  String data = client.readStringUntil('\n');

  // Set JSON Size in Buffer
  StaticJsonDocument<64> filter;

  // Setup JSON Filter
  JsonObject filter_0 = filter.createNestedObject();
  filter_0["pair"] = true;
  filter_0["price"] = true;
  filter_0["percentChange24h"] = true;

  // Build JSON Doc
  StaticJsonDocument<128> doc;

  // Handle any deserialization errors
  DeserializationError error = deserializeJson(doc, data, DeserializationOption::Filter(filter));

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Set our received data variables
  JsonObject root_0 = doc[0];
  const char* data_pair = root_0["pair"]; // "SHIBUSD"
  const char* data_price = root_0["price"]; // "0"
  auto data_change = root_0["percentChange24h"].as<float>(); // "0.0000"

  // Convert data_change to percent
  float changePercent = (data_change * 100);

  if (data != "") {

    // Serial Monitor
    Serial.println("\n== == == == == == ==");
    Serial.print("Price: ");
    Serial.println(data_price);
    Serial.print("1-Hour Change: ");
    Serial.println(String(changePercent) + "%");

    // Update the display
    updatePrice(coin, target, data_price, changePercent);

  } else {

    // Serial Monitor
    Serial.println(" ========== ");
    Serial.print("API Error: ");

    String apiError ("JSON Invalid");

    // Show error on display
    displayError("API ERROR", apiError);

  }
}

// Update the price and price change on the display
void updatePrice(String base, String target, String price, float change) {

  // Clear Display Buffer
  clearDisplay();

  // Set ticker
  display.setFont();
  display.setTextColor(BLACK, WHITE); // Inverted Display White BG, BLK Text
  display.setCursor(1, 0);
  display.print(base);
  display.print(" => ");
  display.print(target);

  // Set the current price
  display.setCursor(1, 16);
  display.setTextColor(WHITE); // Revert to dark BG, White text
  display.setFont(&FreeSansBold9pt7b);
  display.print("$ ");
  display.print(price.substring(0, 11));
  display.setFont();

  // Set the 1-hour change
  display.setCursor(1, 25);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Change: " + String(change));
  display.println(" %");

  // Update LED's
  updateLed(change);

  // Update the display
  updateDisplay();
}

// Display the error passed on the display
void displayError(String type, String e) {

  // Flash Red LED to Alert of Error
  flashNeg(5);

  // Clear Display Buffer
  clearDisplay();

  // Set Title
  display.setTextColor(BLACK, WHITE); // Inverted Display White BG, BLK Text
  display.setCursor(1, 0);
  display.print(type);

  // Show Error
  display.setCursor(1, 11);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print(e);

  display.setCursor(1, 21);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print("Retrying in 15 sec");

  // Update the display
  updateDisplay();

  // Display error for 15 seconds.
  delay(15000);

}

// Setup LED's
void setupLeds() {

  pinMode (ONBOARDLED, OUTPUT); // Onboard LED
  onboardLedStatus = false; // Set initial status

  pinMode(posLed, OUTPUT);  // Green LED
  posLedStatus = false; // Set intial status

  pinMode(negLed, OUTPUT); // Red LED
  negLedStatus = false; // Set initial status

  pinMode(infoLed, OUTPUT); // Blue LED
  infoLedStatus = false;  // Set initial status
}

// Onboard LED on
void onboardLed() {
  switch (onboardLedStatus) {
    case true: // if the led status is true (on)
      digitalWrite (ONBOARDLED, HIGH); // Switch off LED
      break;

    default: // otherwise...
      digitalWrite (ONBOARDLED, LOW); // Switch on LED
      break;
  }
}

// Red LED on
void negOn() {
  allOff();
  negLedStatus = true;
  digitalWrite(negLed, HIGH);
}

// Red LED off
void negOff() {
  negLedStatus = false;
  digitalWrite(negLed, LOW);
}

// Green LED on
void posOn() {
  allOff();
  posLedStatus = true;
  digitalWrite(posLed, HIGH);
}

// Green LED off
void posOff() {
  posLedStatus = false;
  digitalWrite(posLed, HIGH);
}

// Blue LED on
void infoOn() {
  allOff();
  infoLedStatus = true;
  digitalWrite(infoLed, HIGH);
}

// Blue LED off
void infoOff() {
  infoLedStatus = false;
  digitalWrite(infoLed, LOW);
}

// Flash Red LED
void flashNeg(int num) {
  for (int count = 0; count < num; count++)
  {
    negOn();
    delay(250);
    negOff();
    delay(250);
  }
}

// Flash Green LED
void flashPos(int num) {

  for (int count = 0; count < num; count++)
  {
    posOn();
    delay(250);
    posOff();
    delay(250);
  }
}

// Flash Blue LED
void flashInfo(int num) {

  for (int count = 0; count < num; count++)
  {
    infoOn();
    delay(250);
    infoOff();
    delay(250);
  }
}

// Cycle LED through RGB
/* Added check if startup for progress bar updates */
void flashRgb(int num, bool splash) {

  if (splash) {

    // 25%
    //drawProgressbar(0, 20, 120, 10, 25);
    //display.display();
    posOn();
    delay(250);

    // 50%
    //drawProgressbar(0, 20, 120, 10, 50);
    //display.display();
    negOn();
    delay(250);

    // 75%
    //drawProgressbar(0, 20, 120, 10, 75);
    //display.display();
    infoOn();
    delay(250);

  } else {
    for (int count = 0; count < num; count++)
    {
      negOn(); // red
      delay(250);
      posOn(); // green
      delay(250);
      infoOn(); // blue
      delay(250);
    }
  }
}

// Turn all LED's off regardless of state
void allOff() {
  digitalWrite(posLed, LOW);
  posLedStatus = false;
  digitalWrite(negLed, LOW);
  negLedStatus = false;
  digitalWrite(infoLed, LOW);
  infoLedStatus = false;
}

// Update LED based on $/% Value
void updateLed(float changeVal) {
  // If change variable begins with a '-' change is negative
  // So, turn on red led
  if (changeVal < 0) {
    // change is negative
    negOn();

    // Otherwise, it's a positive value, turn on green led
  } else {
    posOn();
  }
}

// Update the Display
void updateDisplay() {
  display.display();
}

// Clear the display
void clearDisplay() {
  display.clearDisplay();
}

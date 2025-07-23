#ifndef API_HANDLER_H
#define API_HANDLER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

extern bool isSplashActive;
extern bool isBootSplash;

#define API_HOST "api.gemini.com"
#define API_PORT 443

class ApiHandler {
private:
    DisplayHandler* display;
    void (*onPriceUpdate)(const String& price, float change) = nullptr;

public:
    ApiHandler(DisplayHandler* disp) : display(disp) {}

    void setUpdateCallback(void (*callback)(const String& price, float change)) {
        onPriceUpdate = callback;
    }

    bool fetchPrice(const String& crypto, const String& fiat) {
        if (isSplashActive) {
            display->showCoinSplash(crypto);
            isSplashActive = false;
        }
        
        WiFiClientSecure client;
        client.setInsecure();  // Don't verify SSL certificate

        Serial.print("Connecting to ");
        Serial.println(API_HOST);

        if (!client.connect(API_HOST, API_PORT)) {
            Serial.println("Connection failed!");
            isBootSplash = false;
            isSplashActive = false;
            display->showError("API Error", "Connection failed!");
            return false;
        }

        Serial.println("Connected to API endpoint");
        // No loading message, keep splash until data is ready

        String apiUrl = "/v1/pricefeed/" + crypto + fiat;
        String request = String("GET ") + apiUrl + " HTTP/1.1\r\n" +
                        "Host: " + API_HOST + "\r\n" +
                        "User-Agent: ESP8266\r\n" +
                        "Connection: close\r\n\r\n";

        Serial.println("Sending request...");
        client.print(request);

        Serial.println("Reading response...");
        // No loading message, keep splash until data is ready
        
        // Wait for data
        unsigned long timeout = millis();
        while (client.available() == 0) {
            if (millis() - timeout > 5000) {
                Serial.println(">>> Client Timeout !");
                client.stop();
                isBootSplash = false;
                isSplashActive = false;
                display->showError("API Error", "Timeout");
                return false;
            }
        }

        // Skip HTTP headers and get status code
        String line;
        int httpCode = 0;
        bool headersParsed = false;
        
        while (client.available()) {
            line = client.readStringUntil('\n');
            line.trim();
            
            // Get HTTP status code from first line
            if (!headersParsed && line.startsWith("HTTP/1.1")) {
                httpCode = line.substring(9, 12).toInt();
                headersParsed = true;
            }
            
            if (line.length() == 0) {
                break;  // Headers finished
            }
        }

        // Check HTTP status code
        if (httpCode != 200) {
            Serial.printf("HTTP Error: %d\n", httpCode);
            display->showError("API Error", "Price pair not available");
            client.stop();
            isBootSplash = false;
            isSplashActive = false;
            return false;
        }

        // Read JSON response
        String jsonData = "";
        while (client.available()) {
            line = client.readStringUntil('\n');
            line.trim();
            if (line.length() > 0) {
                jsonData = line;
                break;
            }
        }

        Serial.println("Raw API Response: " + jsonData);

        StaticJsonDocument<192> doc;
        DeserializationError error = deserializeJson(doc, jsonData);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            isBootSplash = false;
            isSplashActive = false;
            display->showError("JSON Error", error.f_str());
            return false;
        }

        JsonObject root_0 = doc[0];
        
        if (!root_0.isNull()) {
            String price = root_0["price"].as<String>();
            float change = root_0["percentChange24h"].as<float>();

            // Check if price is valid (not empty and not "0" or "0.00")
            if (price != "" && price.toFloat() > 0.0) {
                if (onPriceUpdate != nullptr) {
                    onPriceUpdate(price, change);
                }
                isBootSplash = false;
                isSplashActive = false;
                return true;
            }
        }

        display->showError("API ERROR", "Price pair not available");
        isBootSplash = false;
        isSplashActive = false;
        return false;
    }
};

#endif // API_HANDLER_H 
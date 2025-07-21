#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

class WiFiHandler {
private:
    const char* ssid;
    const char* password;
    DisplayHandler* display;
    LedHandler* led;

public:
    WiFiHandler(const char* wifi_ssid, const char* wifi_password, DisplayHandler* disp, LedHandler* ledHandler) 
        : ssid(wifi_ssid), password(wifi_password), display(disp), led(ledHandler) {}

    bool begin() {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, password);
        Serial.print("Connecting to WiFi ..");

        int attempts = 0;
        const int maxAttempts = 20;  // 20 * 500ms = 10 seconds timeout

        while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
            Serial.print('.');
            display->showWiFiConnecting(attempts + 1, maxAttempts);
            delay(500);
            attempts++;
        }

        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("\nWiFi Connection Failed!");
            display->showWiFiError(ssid);
            
            // Visual error indication
            for (int i = 0; i < 3; i++) {
                led->flashNeg(2);
                delay(500);
            }
            
            return false;
        }

        Serial.println(WiFi.localIP());
        display->showWiFiSuccess(WiFi.localIP());
        led->flashPos(2);  // Success indication
        delay(2000);  // Show connection info for 2 seconds
        
        return true;
    }

    void setupOTA(const char* hostname = "DogeTickler") {
        ArduinoOTA.setHostname(hostname);
        
        ArduinoOTA.onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH) {
                type = "sketch";
            } else {
                type = "filesystem";
            }
            Serial.println("Start updating " + type);
        });
        
        ArduinoOTA.onEnd([]() {
            Serial.println("\nEnd");
        });
        
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });
        
        ArduinoOTA.onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });

        ArduinoOTA.begin();
        Serial.println("OTA Ready");
    }

    void handleOTA() {
        ArduinoOTA.handle();
    }
};

#endif // WIFI_HANDLER_H 
#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

class WiFiHandler {
private:
    const char* ap_ssid = "DogeTicker_AP";
    const char* ap_password = "12345678";  // Default AP password
    DisplayHandler* display;
    LedHandler* led;
    DNSServer dnsServer;
    AsyncWebServer* server;
    bool isConfigured = false;
    String storedSSID;
    String storedPassword;
    unsigned long lastConnectionAttempt = 0;
    const unsigned long CONNECTION_TIMEOUT = 20000; // 20 seconds timeout
    const unsigned long RECONNECT_INTERVAL = 30000; // 30 seconds between reconnection attempts

    bool loadCredentials() {
        if (!LittleFS.exists("/wifi.json")) {
            return false;
        }

        File file = LittleFS.open("/wifi.json", "r");
        if (!file) {
            return false;
        }

        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error) {
            return false;
        }

        storedSSID = doc["ssid"].as<String>();
        storedPassword = doc["password"].as<String>();
        return true;
    }

    bool saveCredentials(const String& ssid, const String& password) {
        StaticJsonDocument<200> doc;
        doc["ssid"] = ssid;
        doc["password"] = password;

        File file = LittleFS.open("/wifi.json", "w");
        if (!file) {
            return false;
        }

        if (serializeJson(doc, file) == 0) {
            file.close();
            return false;
        }

        file.close();
        return true;
    }

    void startAP() {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(ap_ssid, ap_password);
        display->showWiFiAP(ap_ssid, ap_password);
        led->flashInfo(2);  // Indicate AP mode

        // Start DNS server for captive portal
        dnsServer.start(53, "*", WiFi.softAPIP());

        // Setup captive portal routes
        setupCaptivePortal();
        
        Serial.println("AP Started");
        Serial.print("AP IP address: ");
        Serial.println(WiFi.softAPIP());
    }

    void setupCaptivePortal() {
        // Debug: List all files in LittleFS
        Serial.println("Listing files in LittleFS:");
        Dir dir = LittleFS.openDir("/");
        while (dir.next()) {
            Serial.print("File: ");
            Serial.println(dir.fileName());
        }

        // Debug: Check if wifi_config.html exists
        if (LittleFS.exists("/wifi_config.html")) {
            Serial.println("wifi_config.html exists");
            File file = LittleFS.open("/wifi_config.html", "r");
            if (file) {
                Serial.print("File size: ");
                Serial.println(file.size());
                file.close();
            }
        } else {
            Serial.println("wifi_config.html does not exist!");
        }

        // Catch-all handler for captive portal
        server->onNotFound([](AsyncWebServerRequest *request) {
            Serial.print("404 Not Found: ");
            Serial.println(request->url());
            request->send(LittleFS, "/wifi_config.html", "text/html");
        });

        // Serve the WiFi configuration page
        server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
            Serial.println("Serving wifi_config.html");
            request->send(LittleFS, "/wifi_config.html", "text/html");
        });

        // Handle WiFi configuration submission
        server->on("/configure", HTTP_POST, [this](AsyncWebServerRequest *request) {
            String ssid = request->arg("ssid");
            String password = request->arg("password");
            
            if (ssid.length() > 0) {
                if (saveCredentials(ssid, password)) {
                    // Send a response that includes a redirect
                    String response = "<html><head><meta http-equiv='refresh' content='2;url=/settings'></head>";
                    response += "<body><h1>WiFi configuration saved!</h1>";
                    response += "<p>Redirecting to settings page...</p>";
                    response += "<script>setTimeout(function() { window.location.href = '/settings'; }, 2000);</script>";
                    response += "</body></html>";
                    request->send(200, "text/html", response);
                    
                    // Start a timer to restart after a delay
                    delay(3000);  // Give time for the redirect to happen
                    ESP.restart();
                } else {
                    request->send(500, "text/plain", "Failed to save configuration");
                }
            } else {
                request->send(400, "text/plain", "SSID is required");
            }
        });

        // Serve static files
        server->serveStatic("/", LittleFS, "/");
    }

public:
    WiFiHandler(DisplayHandler* disp, LedHandler* ledHandler, AsyncWebServer* webServer) 
        : display(disp), led(ledHandler), server(webServer) {}

    bool begin() {
        if (!loadCredentials()) {
            Serial.println("No WiFi credentials found, starting AP mode");
            startAP();
            return false;
        }

        return connectToWiFi();
    }

    bool connectToWiFi() {
        if (storedSSID.length() == 0) {
            return false;
        }

        WiFi.mode(WIFI_STA);
        WiFi.begin(storedSSID.c_str(), storedPassword.c_str());
        Serial.print("Connecting to WiFi: ");
        Serial.println(storedSSID);

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
            display->showWiFiError(storedSSID.c_str());
            led->flashNeg(2);
            return false;
        }

        Serial.println(WiFi.localIP());
        display->showWiFiSuccess(WiFi.localIP());
        led->flashPos(2);
        return true;
    }

    void handle() {
        if (WiFi.getMode() == WIFI_AP) {
            dnsServer.processNextRequest();
        } else if (WiFi.status() != WL_CONNECTED) {
            unsigned long currentMillis = millis();
            if (currentMillis - lastConnectionAttempt >= RECONNECT_INTERVAL) {
                lastConnectionAttempt = currentMillis;
                if (!connectToWiFi()) {
                    // If reconnection fails, start AP mode
                    startAP();
                }
            }
        }
    }

    void setupOTA(const char* hostname = "DogeTicker") {
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
};

#endif // WIFI_HANDLER_H 
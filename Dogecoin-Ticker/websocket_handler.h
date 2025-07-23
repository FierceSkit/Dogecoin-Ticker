#ifndef WEBSOCKET_HANDLER_H
#define WEBSOCKET_HANDLER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Arduino_JSON.h>

extern bool isSplashActive;

class WebSocketHandler {
private:
    AsyncWebSocket ws;
    String currentCrypto;
    String currentCurrency;
    unsigned long* previousFetch;

public:
    WebSocketHandler(const char* wsPath = "/ws") : ws(wsPath) {}

    void begin(AsyncWebServer* server, String& crypto, String& currency, unsigned long& prevFetch) {
        currentCrypto = crypto;
        currentCurrency = currency;
        previousFetch = &prevFetch;
        
        ws.onEvent(std::bind(&WebSocketHandler::handleWebSocketEvent, this,
            std::placeholders::_1, std::placeholders::_2,
            std::placeholders::_3, std::placeholders::_4,
            std::placeholders::_5, std::placeholders::_6));
        
        server->addHandler(&ws);
    }

    void cleanupClients() {
        ws.cleanupClients();
    }

    void notifyClients(const String& state) {
        ws.textAll(state);
    }

    String getCurrentStates() {
        JSONVar jsonData;
        jsonData["states"][0]["sender"] = "esp8266";
        jsonData["states"][0]["currentCurrency"] = currentCurrency;
        jsonData["states"][0]["currentCrypto"] = currentCrypto;
        return JSON.stringify(jsonData);
    }

private:
    void handleWebSocketEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, 
                            AwsEventType type, void* arg, uint8_t* data, size_t len) {
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

    void handleWebSocketMessage(void* arg, uint8_t* data, size_t len) {
        AwsFrameInfo* info = (AwsFrameInfo*)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            Serial.print("Received Websocket Message: ");
            Serial.print((char*)data);
            Serial.print("\n");

            if (strcmp((char*)data, "getCurrentStates") == 0) {
                notifyClients(getCurrentStates());
            } else {
                StaticJsonDocument<192> doc;
                DeserializationError error = deserializeJson(doc, (char*)data);

                if (error) {
                    Serial.print(F("deserializeJson() failed: "));
                    Serial.println(error.f_str());
                    return;
                }

                JsonObject states_0 = doc["states"][0];
                const char* sender = states_0["sender"];
                String newCurrency = states_0["currentCurrency"];
                String newCrypto = states_0["currentCrypto"];

                if (strcmp(sender, "client") == 0) {
                    Serial.println("Received message from client.");
                    currentCrypto = newCrypto;
                    currentCurrency = newCurrency;
                    *previousFetch = 0;
                    isSplashActive = true;
                    notifyClients(getCurrentStates());
                }
            }
        }
    }
};

#endif // WEBSOCKET_HANDLER_H 
#ifndef LED_HANDLER_H
#define LED_HANDLER_H

#include <Arduino.h>

class LedHandler {
private:
    const int onboardLedPin;
    const int posLedPin;
    const int negLedPin;
    const int infoLedPin;
    
    bool onboardLedStatus = false;
    bool posLedStatus = false;
    bool negLedStatus = false;
    bool infoLedStatus = false;

public:
    LedHandler(int onboard, int pos, int neg, int info) 
        : onboardLedPin(onboard), posLedPin(pos), negLedPin(neg), infoLedPin(info) {}

    void begin() {
        pinMode(onboardLedPin, OUTPUT);
        pinMode(posLedPin, OUTPUT);
        pinMode(negLedPin, OUTPUT);
        pinMode(infoLedPin, OUTPUT);
        allOff();
    }

    void allOff() {
        digitalWrite(posLedPin, LOW);
        posLedStatus = false;
        digitalWrite(negLedPin, LOW);
        negLedStatus = false;
        digitalWrite(infoLedPin, LOW);
        infoLedStatus = false;
        digitalWrite(onboardLedPin, HIGH); // Onboard LED is active LOW
        onboardLedStatus = false;
    }

    void onboardLed() {
        onboardLedStatus = !onboardLedStatus;
        digitalWrite(onboardLedPin, !onboardLedStatus); // Inverted because onboard LED is active LOW
    }

    void negOn() {
        allOff();
        negLedStatus = true;
        digitalWrite(negLedPin, HIGH);
    }

    void negOff() {
        negLedStatus = false;
        digitalWrite(negLedPin, LOW);
    }

    void posOn() {
        allOff();
        posLedStatus = true;
        digitalWrite(posLedPin, HIGH);
    }

    void posOff() {
        posLedStatus = false;
        digitalWrite(posLedPin, LOW);
    }

    void infoOn() {
        allOff();
        infoLedStatus = true;
        digitalWrite(infoLedPin, HIGH);
    }

    void infoOff() {
        infoLedStatus = false;
        digitalWrite(infoLedPin, LOW);
    }

    void flashNeg(int num) {
        for (int count = 0; count < num; count++) {
            negOn();
            delay(250);
            negOff();
            delay(250);
        }
    }

    void flashPos(int num) {
        for (int count = 0; count < num; count++) {
            posOn();
            delay(250);
            posOff();
            delay(250);
        }
    }

    void flashInfo(int num) {
        for (int count = 0; count < num; count++) {
            infoOn();
            delay(250);
            infoOff();
            delay(250);
        }
    }

    void flashRgb(int num, bool splash = false) {
        if (splash) {
            posOn();
            delay(250);
            negOn();
            delay(250);
            infoOn();
            delay(250);
        } else {
            for (int count = 0; count < num; count++) {
                negOn();
                delay(250);
                posOn();
                delay(250);
                infoOn();
                delay(250);
            }
        }
    }

    void updateLed(float changeVal) {
        if (changeVal < 0) {
            negOn();
        } else {
            posOn();
        }
    }
};

#endif // LED_HANDLER_H 
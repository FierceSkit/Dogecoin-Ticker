#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <Arduino.h>

// Display dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// Pin definitions
#define ONBOARDLED 2
#define posLed 14
#define negLed 12
#define infoLed 13
#define OLED_RESET -1
#define OLED_ADDR 0x3C

// Available cryptocurrencies
const int NUM_CRYPTOCURRENCIES = 3;
extern const String CRYPTOCURRENCIES[NUM_CRYPTOCURRENCIES];

// Available fiat currencies
const int NUM_FIAT_CURRENCIES = 5;
extern const String FIAT_CURRENCIES[NUM_FIAT_CURRENCIES];

// Timing constants
const unsigned long PREVIEW_DURATION = 2000;
const long fetchInterval = 30000;

#endif // DEFINITIONS_H 
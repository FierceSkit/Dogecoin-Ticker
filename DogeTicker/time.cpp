#include <Arduino.h>
#include "time.h"

void TimeHandler::begin() {
    // Initialize time library
    setTime(0);
}

void TimeHandler::update(unsigned long epoch) {
    setTime(epoch);
}

String TimeHandler::getFormattedTime() {
    char timeStr[9];
    sprintf(timeStr, "%02d:%02d:%02d", hour(), minute(), second());
    return String(timeStr);
}

String TimeHandler::getFormattedDate() {
    char dateStr[11];
    sprintf(dateStr, "%04d-%02d-%02d", year(), month(), day());
    return String(dateStr);
} 
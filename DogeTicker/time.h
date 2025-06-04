#ifndef TIME_H
#define TIME_H

#include <Arduino.h>
#include <TimeLib.h>

class TimeHandler {
public:
    static void begin();
    static void update(unsigned long epoch);
    static String getFormattedTime();
    static String getFormattedDate();
};

#endif // TIME_H 
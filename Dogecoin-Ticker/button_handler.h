#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>

// Button Definitions
#define BUTTON_PIN 0  // GPIO0 (D3)
#define LONG_PRESS_DURATION 1000  // Duration for long press in milliseconds

class ButtonHandler {
  private:
    unsigned long lastDebounceTime = 0;
    unsigned long debounceDelay = 50;    // Debounce time in milliseconds
    unsigned long pressStartTime = 0;     // When the button was pressed
    bool isPressing = false;              // Track if button is being held
    int lastButtonState = HIGH;          // Previous button reading
    int buttonState = HIGH;              // Current stable button state
    
    // Callback function pointers
    void (*onShortPress)() = nullptr;
    void (*onLongPress)() = nullptr;
    void (*onPressing)() = nullptr;      // Called while button is being held

  public:
    ButtonHandler() {}

    void begin() {
      pinMode(BUTTON_PIN, INPUT_PULLUP);  // Enable internal pull-up resistor
    }

    void setCallbacks(void (*shortPress)(), void (*longPress)() = nullptr, void (*pressing)() = nullptr) {
      onShortPress = shortPress;
      onLongPress = longPress;
      onPressing = pressing;
    }

    void handle() {
      // Read the current button state
      int reading = digitalRead(BUTTON_PIN);

      // If the button state changed, reset the debounce timer
      if (reading != lastButtonState) {
        lastDebounceTime = millis();
      }

      // Check if enough time has passed since the last state change
      if ((millis() - lastDebounceTime) > debounceDelay) {
        // If the button state has changed and is stable
        if (reading != buttonState) {
          buttonState = reading;

          // Button has been pressed (LOW due to pull-up)
          if (buttonState == LOW) {
            pressStartTime = millis();
            isPressing = true;
          }
          // Button has been released
          else if (isPressing) {
            isPressing = false;
            unsigned long pressDuration = millis() - pressStartTime;
            
            // Determine if it was a short or long press
            if (pressDuration >= LONG_PRESS_DURATION) {
              if (onLongPress != nullptr) onLongPress();
            } else {
              if (onShortPress != nullptr) onShortPress();
            }
          }
        }
        
        // If button is being held down, call the pressing callback
        if (isPressing && buttonState == LOW) {
          if (onPressing != nullptr) onPressing();
        }
      }

      lastButtonState = reading;
    }
};

#endif // BUTTON_HANDLER_H 
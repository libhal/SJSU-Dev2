// This provides the functions to manipulate the numbered LEDs
//
// The initializaiton function takes care of setting up the pins to function as
// GPIO outputs. When an output is set to LOW, the LED lights up, when the
// output is set to HIGH the LED will be off.

#pragma once

#include <cstdint>

class OnBoardLedInterface {
 public:
           virtual void Initialize()                                 = 0;
           virtual void LedOn(uint8_t led_number)                    = 0;
           virtual void LedOff(uint8_t led_number)                   = 0;
           virtual void LedSet(uint8_t led_number, bool on)          = 0;
           virtual void LedToggle(uint8_t led_number)                = 0;
           virtual void LedSetAll(uint8_t value)                     = 0;
           virtual uint8_t LedGetValues(void)                        = 0;
};

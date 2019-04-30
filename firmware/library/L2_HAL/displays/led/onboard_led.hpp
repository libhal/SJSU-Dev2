// This provides the functions to manipulate the numbered LEDs
//
// The initializaiton function takes care of setting up the pins to function as
// GPIO outputs. When an output is set to LOW, the LED lights up, when the
// output is set to HIGH the LED will be off.

#pragma once

#include <cstdint>

#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "utility/log.hpp"

// TODO(#274): Delete this file and add this to the appropriate board.h file.
//
// Example:
//
//    ParallelBus & OnboardLeds()
//    {
//      static sjsu::lpc40xx::Gpio led0 = sjsu::lpc40xx::Gpio(0,0);
//      static sjsu::lpc40xx::Gpio led1 = sjsu::lpc40xx::Gpio(0,1);
//      static sjsu::lpc40xx::Gpio led2 = sjsu::lpc40xx::Gpio(0,2);
//      static sjsu::lpc40xx::Gpio led3 = sjsu::lpc40xx::Gpio(0,3);
//      // takes initializer_list<sjsu::Gpio&>
//      static ParallelBus onboard_leds({ led0, led1, led2, led3 });
//      return return onboard_leds;
//    }

namespace sjsu
{
class OnBoardLed
{
 public:
  enum class LightState : bool
  {
    kOn  = true,
    kOff = false
  };
  // Initialize takes the array of Gpios, sets each one to an output, and
  // then turns off all of the leds by setting the output high.
  void Initialize()
  {
    for (uint8_t i = 0; i < 4; i++)
    {
      led[i].SetAsOutput();
      led[i].SetHigh();
    }
  }

  void On(uint8_t led_number)
  {
    SJ2_ASSERT_FATAL(led_number < 4,
                     "Input Led number can't be greater than 3, input = %d.\n",
                     led_number);
    led[led_number].SetLow();
  }

  void Off(uint8_t led_number)
  {
    SJ2_ASSERT_FATAL(led_number < 4,
                     "Input Led number can't be greater than 3, input = %d.\n",
                     led_number);
    led[led_number].SetHigh();
  }

  void Set(uint8_t led_number, LightState state = LightState::kOn)
  {
    SJ2_ASSERT_FATAL(led_number < 4,
                     "Input Led number can't be greater than 3, input = %d.\n",
                     led_number);
    if (state == LightState::kOn)
    {
      On(led_number);
    }
    else
    {
      Off(led_number);
    }
  }

  void Toggle(uint8_t led_number)
  {
    SJ2_ASSERT_FATAL(led_number < 4,
                     "Input Led number can't be greater than 3, input = %d.\n",
                     led_number);
    led[led_number].Toggle();
  }

  // This function takes in the 4 least significant bits from value, and sets
  // the led to be ON or OFF. The least significant bit corresponds to LED0,
  // next least significant corresponds to LED1, etc. ON =1, OFF =0. The four
  // most significant bits will be unused.
  void SetAll(uint8_t value)
  {
    for (uint8_t i = 0; i < 4; i++)
    {
      if ((value >> i) & 1)
      {
        Set(i, LightState::kOn);
      }
      else
      {
        Set(i, LightState::kOff);
      }
    }
  }
  // This function will return the states of LED0-LED3, going from bit 0
  // represnting LED 0, to bit 3 representing LED3. Only the four least
  // significant bits will be used. The four most significant bits will be 0s.
  uint8_t GetStates()
  {
    uint32_t led_states = 0x0000;
    for (uint8_t i = 0; i < 4; i++)
    {
      led_states |= led[i].Read() << i;
    }
    return static_cast<uint8_t>(led_states);
  }

 protected:
  sjsu::lpc40xx::Gpio led[4] = {
    sjsu::lpc40xx::Gpio(2, 3),
    sjsu::lpc40xx::Gpio(1, 26),
    sjsu::lpc40xx::Gpio(1, 24),
    sjsu::lpc40xx::Gpio(1, 18),
  };
};
}  // namespace sjsu

// This provides the functions to manipulate the numbered LEDs
//
// The initializaiton function takes care of setting up the pins to function as
// GPIO outputs. When an output is set to LOW, the LED lights up, when the
// output is set to HIGH the LED will be off.

#pragma once

#include <cstdint>
#include "L1_Drivers/gpio.hpp"
#include "L2_Utilities/macros.hpp"

class OnBoardLedInterface
{
 public:
  enum class LightState : bool
  {
    kOn  = true,
    kOff = false
  };
  virtual void Initialize()                              = 0;
  virtual void On(uint8_t led_number)                    = 0;
  virtual void Off(uint8_t led_number)                   = 0;
  virtual void Set(uint8_t led_number, LightState state) = 0;
  virtual void Toggle(uint8_t led_number)                = 0;
  virtual void SetAll(uint8_t value)                     = 0;
  virtual uint8_t GetStates(void)                        = 0;
};

class OnBoardLed : public OnBoardLedInterface
{
 public:
  // Initialize takes the array of Gpios, sets each one to an output, and
  // then turns off all of the leds by setting the output high.
  void Initialize(void) override
  {
    for (uint8_t i = 0; i < 4; i++)
    {
      led[i].SetAsOutput();
      led[i].SetHigh();
    }
  }

  void On(uint8_t led_number) override
  {
    SJ2_ASSERT_FATAL(led_number < 4,
                     "Input Led number can't be greater than 3, input = %d.\n",
                     led_number);
    led[led_number].SetLow();
  }

  void Off(uint8_t led_number) override
  {
    SJ2_ASSERT_FATAL(led_number < 4,
                     "Input Led number can't be greater than 3, input = %d.\n",
                     led_number);
    led[led_number].SetHigh();
  }

  void Set(uint8_t led_number,
           OnBoardLedInterface::LightState state = LightState::kOn) override
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

  void Toggle(uint8_t led_number) override
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
  void SetAll(uint8_t value) override
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
  uint8_t GetStates(void) override
  {
    uint32_t led_states = 0x0000;
    for (uint8_t i = 0; i < 4; i++)
    {
      led_states |= led[i].Read() << i;
    }
    return static_cast<uint8_t>(led_states);
  }

 protected:
  Gpio led[4] = { Gpio(1, 1), Gpio(1, 8), Gpio(1, 10), Gpio(1, 15) };
};

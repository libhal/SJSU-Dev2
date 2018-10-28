#pragma once

#include <cstdint>

#include "L1_Drivers/gpio.hpp"

class ButtonInterface
{
 public:
  virtual void Initialize(void)                                     = 0;
  virtual bool Released(void)                                       = 0;
  virtual bool Pressed(void)                                        = 0;
  virtual void ResetState(void)                                     = 0;
  virtual void InvertButtonSignal(bool enable_invert_signal = true) = 0;
};

class Button : public ButtonInterface, public Gpio
{
 public:
  constexpr Button(uint8_t port_num, uint8_t pin_num) : Gpio(port_num, pin_num)
  {
  }

  void Initialize(void) override
  {
    SetAsInput();
    SetMode(PinInterface::Mode::kPullDown);
  }

  bool Released(void) override
  {
    bool result = false;
    if (Read() && !was_pressed_)
    {
      was_pressed_ = true;
    }
    else if (!Read() && was_pressed_)
    {
      result      = true;
      was_pressed_ = false;
    }
    return result;
  }

  bool Pressed(void) override
  {
    bool result = false;
    if (Read() && !was_pressed_)
    {
      result      = true;
      was_pressed_ = true;
    }
    else if (!Read() && was_pressed_)
    {
      was_pressed_ = false;
    }
    return result;
  }

  void ResetState(void) override
  {
    was_pressed_ = false;
  }

  void InvertButtonSignal(bool enable_invert_signal = true) override
  {
    SetAsActiveLow(enable_invert_signal);
  }

  ~Button() {}

 private:
  bool was_pressed_ = false;
};

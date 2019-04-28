#pragma once

#include <cstdint>

#include "L1_Peripheral/gpio.hpp"

namespace sjsu
{
class Button
{
 public:
  constexpr explicit Button(const Gpio & button)
      : button_(button),
        was_pressed_(false),
        was_released_(false)
  {
  }

  virtual void Initialize()
  {
    button_.SetDirection(Gpio::Direction::kInput);
    button_.GetPin().SetMode(Pin::Mode::kPullDown);
  }
  virtual bool Released()
  {
    bool result = false;
    if (button_.Read() && !was_pressed_)
    {
      was_pressed_ = true;
    }
    else if (!button_.Read() && was_pressed_)
    {
      result       = true;
      was_pressed_ = false;
    }
    return result;
  }

  virtual bool Pressed()
  {
    bool result = false;
    if (!button_.Read() && !was_released_)
    {
      was_released_ = true;
    }
    else if (button_.Read() && was_released_)
    {
      result        = true;
      was_released_ = false;
    }
    return result;
  }

  virtual void ResetState()
  {
    was_pressed_  = false;
    was_released_ = false;
  }
  virtual const Gpio & GetGpio()
  {
    return button_;
  }

 private:
  const Gpio & button_;
  bool was_pressed_;
  bool was_released_;
};
}  // namespace sjsu

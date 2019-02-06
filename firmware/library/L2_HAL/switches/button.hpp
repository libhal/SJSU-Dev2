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
  virtual GpioInterface & GetGpio()                                 = 0;
};

class Button : public ButtonInterface
{
 public:
  constexpr Button(uint8_t port, uint8_t pin)
      : button_(&button_gpio_),
        button_gpio_(port, pin),
        was_pressed_(false),
        was_released_(false)
  {
  }
  constexpr explicit Button(GpioInterface * button)
      : button_(button),
        button_gpio_(0, 0),
        was_pressed_(false),
        was_released_(false)
  {
  }

  void Initialize(void) override
  {
    button_->SetAsInput();
    button_->GetPin().SetMode(PinInterface::Mode::kPullDown);
    InvertButtonSignal(false);
  }
  void InvertButtonSignal(bool enable_invert_signal = true) override
  {
    button_->GetPin().SetAsActiveLow(enable_invert_signal);
  }
  bool Released(void) override
  {
    bool result = false;
    if (button_->Read() && !was_pressed_)
    {
      was_pressed_ = true;
    }
    else if (!button_->Read() && was_pressed_)
    {
      result       = true;
      was_pressed_ = false;
    }
    return result;
  }

  bool Pressed(void) override
  {
    bool result = false;
    if (!button_->Read() && !was_released_)
    {
      was_released_ = true;
    }
    else if (button_->Read() && was_released_)
    {
      result        = true;
      was_released_ = false;
    }
    return result;
  }

  void ResetState(void) override
  {
    was_pressed_  = false;
    was_released_ = false;
  }
  GpioInterface & GetGpio() override
  {
    return *button_;
  }

 private:
  GpioInterface * button_;
  Gpio button_gpio_;
  bool was_pressed_;
  bool was_released_;
};

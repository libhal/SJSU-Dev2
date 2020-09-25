#pragma once

#include <cstdint>

#include "L1_Peripheral/gpio.hpp"
#include "module.hpp"

namespace sjsu
{
/// An button class that can tell you if a button has been pressed or released
/// using a sjsu::Gpio object. This class does not use interrupt and requires
/// constant polling in order to work. This is useful for situations where a
/// gpio does not have interrupt capability, or interrupt capability is not
/// desired.
class Button : public Module
{
 public:
  /// Button Constructor
  constexpr explicit Button(sjsu::Gpio & button) : button_(button) {}

  void ModuleInitialize() override
  {
    button_.Initialize();
  }

  void ModuleEnable(bool enable = true) override
  {
    button_.Enable(enable);

    if (enable)
    {
      button_.SetAsInput();
    }
  }

  /// Call this function continuously to detect if a button has been released.
  /// This method must be called at least twice before it can return true for a
  /// "released" state.
  ///
  /// How it works:
  /// When called, this method checks what the previous state of the gpio signal
  /// was compared to right now. If the state of the button was HIGH previously
  /// and is now LOW, that means that a user's finger was previously pressing on
  /// the button, and has since released the button. This is also know as a
  /// falling edge signal.
  ///
  /// Usage:
  /// In order for this to work, the Released() method needs to be called
  /// periodically to check that the state of the pin has changed. If it is not
  /// called fast enough, it may miss a transition of the signal from HIGH to
  /// LOW.
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

  /// Same documentation as Released() but for the Pressed() state.
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

  /// Resets the internal state of the button controller.
  virtual void ResetState()
  {
    was_pressed_  = false;
    was_released_ = false;
  }
  /// @returns a reference to the internal sjsu::Gpio object.
  virtual const sjsu::Gpio & GetGpio()
  {
    return button_;
  }

 private:
  sjsu::Gpio & button_;
  bool was_pressed_  = false;
  bool was_released_ = false;
};
}  // namespace sjsu

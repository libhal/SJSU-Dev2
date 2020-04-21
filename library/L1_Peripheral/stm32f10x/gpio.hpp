#pragma once

#include <cstdint>

#include "L1_Peripheral/gpio.hpp"
#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/stm32f10x/pin.hpp"
#include "utility/status.hpp"

namespace sjsu::stm32f10x
{
/// An abstract interface for General Purpose I/O
class Gpio : public sjsu::Gpio
{
 public:
  /// @param port - must be a capitol letter from 'A' to 'G'
  /// @param pin - must be between 0 to 15
  constexpr Gpio(uint8_t port, uint8_t pin) : pin_{ port, pin } {}

  void SetDirection(Direction direction) const override
  {
    // Power up the gpio peripheral corresponding to this gpio port.
    // NOTE: Initialize() on pin is safe to execute multiple times even if the
    // peripheral is already powered on. If the peripheral is already powered
    // on, then nothing will change.
    pin_.Initialize();

    if (direction == Direction::kInput)
    {
      // Using the `SetFloating()` method here will do the work of setting the
      // pin to an input as well as setting the pin to the its reset state as
      // defined within the RM0008 user manual for the STM32F10x.
      pin_.SetFloating();
    }
    else
    {
      // Setting pin function to 0 converts pin to an output GPIO pin.
      pin_.SetPinFunction(0);
    }
  }

  void Set(State output) const override
  {
    if (output == State::kHigh)
    {
      // The first 16 bits of the register set the output state
      pin_.Port()->BSRR = (1 << pin_.GetPin());
    }
    else
    {
      // The last 16 bits of the register reset the output state
      pin_.Port()->BSRR = (1 << (pin_.GetPin() + 16));
    }
  }

  void Toggle() const override
  {
    pin_.Port()->ODR ^= (1 << pin_.GetPin());
  }

  bool Read() const override
  {
    return bit::Read(pin_.Port()->IDR, pin_.GetPin());
  }

  const sjsu::Pin & GetPin() const override
  {
    return pin_;
  }

  void AttachInterrupt(InterruptCallback, Edge) override
  {
    LogError("Not implemented");
  }

  void DetachInterrupt() const override
  {
    LogError("Not implemented");
  }

 private:
  sjsu::stm32f10x::Pin pin_;
};
}  // namespace sjsu::stm32f10x

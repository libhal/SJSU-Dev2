#pragma once

#include <cstdint>

#include "L1_Peripheral/gpio.hpp"
#include "L1_Peripheral/stm32f4xx/pin.hpp"
#include "utility/status.hpp"

namespace sjsu::stm32f4xx
{
/// An abstract interface for General Purpose I/O
class Gpio : public sjsu::Gpio
{
 public:
  /// @param port - must be a capitol letter from 'A' to 'I'
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
      // RM0090 p.281
      //
      // --> 00: Input (reset state)
      //     01: General purpose output mode
      //     10: Alternate function mode
      //     11: Analog mode
      pin_.Port()->MODER = bit::Insert(pin_.Port()->MODER, 0b00, pin_.Mask());
    }
    else
    {
      // RM0090 p.281
      //
      //     00: Input (reset state)
      // --> 01: General purpose output mode
      //     10: Alternate function mode
      //     11: Analog mode
      pin_.Port()->MODER = bit::Insert(pin_.Port()->MODER, 0b01, pin_.Mask());
    }
  }

  void Set(State output) const override
  {
    if (output == State::kLow)
    {
      pin_.Port()->BSRRH = static_cast<uint16_t>(1 << pin_.GetPin());
    }
    else
    {
      pin_.Port()->BSRRL = static_cast<uint16_t>(1 << pin_.GetPin());
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
    LOG_INFO("Not Implemented");
  }

  void DetachInterrupt() const override
  {
    LOG_INFO("Not Implemented");
  }

 private:
  sjsu::stm32f4xx::Pin pin_;
};
}  // namespace sjsu::stm32f4xx

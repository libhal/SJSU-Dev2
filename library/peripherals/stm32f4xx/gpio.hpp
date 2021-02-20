#pragma once

#include <cstdint>

#include "peripherals/gpio.hpp"
#include "peripherals/stm32f4xx/pin.hpp"
#include "utility/error_handling.hpp"

namespace sjsu::stm32f4xx
{
/// An abstract interface for General Purpose I/O
class Gpio : public sjsu::Gpio
{
 public:
  /// @param port - must be a capitol letter from 'A' to 'I'
  /// @param pin - must be between 0 to 15
  constexpr Gpio(uint8_t port, uint8_t pin) : pin_{ port, pin } {}

  void ModuleInitialize() override
  {
    pin_.Initialize();
  }

  void SetDirection(Direction direction) override
  {
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

  void Set(State output) override
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

  void Toggle() override
  {
    pin_.Port()->ODR ^= (1 << pin_.GetPin());
  }

  bool Read() override
  {
    return bit::Read(pin_.Port()->IDR, pin_.GetPin());
  }

  sjsu::Pin & GetPin() override
  {
    return pin_;
  }

  void AttachInterrupt(InterruptCallback, Edge) override
  {
    throw Exception(std::errc::operation_not_supported, "");
  }

  void DetachInterrupt() override
  {
    throw Exception(std::errc::operation_not_supported, "");
  }

 private:
  sjsu::stm32f4xx::Pin pin_;
};

template <int port, int pin_number>
inline Gpio & GetGpio()
{
  static_assert(
      ('A' <= port && port <= 'I') && (0 <= pin_number && pin_number <= 15),
      SJ2_ERROR_MESSAGE_DECORATOR("stm32f4xx: Port must be between 'A' and 'I' "
                                  "and pin must be between 0 and 15!\n"));

  static Gpio gpio(port, pin_number);
  return gpio;
}
}  // namespace sjsu::stm32f4xx

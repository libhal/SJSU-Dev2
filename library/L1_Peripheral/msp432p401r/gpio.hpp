#pragma once

#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "L1_Peripheral/gpio.hpp"
#include "L1_Peripheral/msp432p401r/pin.hpp"
#include "utility/bit.hpp"

namespace sjsu
{
namespace msp432p401r
{
/// GPIO implementation for the MSP432P401R platform.
class Gpio final : public sjsu::Gpio
{
 public:
  /// @param port The port number. The capitol letter 'J' should be used is the
  ///             desired port is port J.
  /// @param pin  The pin number.
  constexpr Gpio(uint8_t port, uint8_t pin) : pin_{ port, pin } {}

  void ModuleInitialize() override {}
  void ModuleEnable(bool = true) override {}

  void SetDirection(Direction direction) override
  {
    constexpr auto kDirectionBit = bit::MaskFromRange(2);
    uint32_t function_code       = 0b00;
    function_code = bit::Insert(function_code, Value(direction), kDirectionBit);
    pin_.ConfigureFunction(static_cast<uint8_t>(function_code));
  }

  void Set(State output) override
  {
    volatile uint8_t * out_register = pin_.RegisterAddress(&pin_.Port()->OUT);
    if (output == State::kHigh)
    {
      *out_register = bit::Set(*out_register, pin_.GetPin());
    }
    else
    {
      *out_register = bit::Clear(*out_register, pin_.GetPin());
    }
  }

  void Toggle() override
  {
    volatile uint8_t * out_register = pin_.RegisterAddress(&pin_.Port()->OUT);
    *out_register                   = bit::Toggle(*out_register, pin_.GetPin());
  }

  bool Read() override
  {
    volatile uint8_t * in_register = pin_.RegisterAddress(&pin_.Port()->IN);
    return bit::Read(*in_register, pin_.GetPin());
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
  msp432p401r::Pin pin_;
};
}  // namespace msp432p401r
}  // namespace sjsu

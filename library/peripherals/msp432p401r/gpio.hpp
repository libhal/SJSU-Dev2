#pragma once

#include "platforms/targets/msp432p401r/msp432p401r.h"
#include "peripherals/gpio.hpp"
#include "peripherals/msp432p401r/pin.hpp"
#include "utility/math/bit.hpp"

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

  void ModuleInitialize() override
  {
    pin_.Initialize();
  }

  void SetDirection(Direction direction) override
  {
    constexpr auto kDirectionBit = bit::MaskFromRange(2);

    pin_.settings.function =
        bit::Value(0).Insert(Value(direction), kDirectionBit).To<uint8_t>();
    pin_.Initialize();
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

template <int port, int pin_number>
inline Gpio & GetGpio()
{
  // NOTE: port can only be 1-10 or 'J'
  static_assert((1 <= port && port <= 10) || (port == 'J'),
                "Port must be 1-10 or J");
  static_assert(pin_number <= 7, "Pin must be between 0 and 7");

  static Gpio gpio(port, pin_number);
  return gpio;
}
}  // namespace msp432p401r
}  // namespace sjsu

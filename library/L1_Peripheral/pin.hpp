#pragma once

#include <cstdint>

namespace sjsu
{
class Pin
{
 public:
  // ==============================
  // Interface Defintions
  // ==============================

  enum class Resistor : uint8_t
  {
    kNone = 0,
    kPullDown,
    kPullUp,
    kRepeater
  };

  constexpr Pin(uint8_t port, uint8_t pin) : port_(port), pin_(pin) {}

  // ==============================
  // Interface Methods
  // ==============================

  /// Set the pin's function
  ///
  /// @param function - pin function code
  virtual void SetPinFunction(uint8_t function) const = 0;
  /// Set pin's resistor pull, setting ot either no resistor pull, pull down,
  /// pull up and repeater.
  ///
  /// @param mode - pin mode
  virtual void SetPull(Resistor mode) const = 0;
  /// Set pin to open drain mode
  ///
  /// @param set_as_open_drain - if false, disable open drain feature, pin
  ///        becomes push-pull (a.k.a totem pole).
  virtual void SetAsOpenDrain(bool set_as_open_drain = true) const = 0;
  /// Set pin as analog mode
  ///
  /// @param set_as_analog - if false, disable analog mode for pin
  virtual void SetAsAnalogMode(bool set_as_analog = true) const    = 0;

  // ==============================
  // Utility Methods
  // ==============================

  uint8_t GetPort() const
  {
    return port_;
  }
  uint8_t GetPin() const
  {
    return pin_;
  }

 protected:
  uint8_t port_;
  uint8_t pin_;
};
}  // namespace sjsu

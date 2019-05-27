#pragma once

#include <cstdint>

namespace sjsu
{
class Pin
{
 public:
  enum class Mode : uint8_t
  {
    kInactive = 0,
    kPullDown,
    kPullUp,
    kRepeater
  };

  constexpr Pin(uint8_t port, uint8_t pin) : port_(port), pin_(pin) {}

  virtual void SetPinFunction(uint8_t function) const              = 0;
  virtual void SetMode(Pin::Mode mode) const              = 0;
  virtual void SetAsOpenDrain(bool set_as_open_drain = true) const = 0;
  virtual void SetAsAnalogMode(bool set_as_analog = true) const    = 0;

  [[gnu::always_inline]] uint8_t GetPort() const {
    return port_;
  }[[gnu::always_inline]] uint8_t GetPin() const
  {
    return pin_;
  }

 protected:
  uint8_t port_;
  uint8_t pin_;
};
}  // namespace sjsu

#pragma once

#include <cstdint>

#include "utility/status.hpp"

namespace sjsu
{
class Pwm
{
 public:
  static constexpr uint32_t kDefaultFrequency = 1'000;

  virtual Status Initialize(
      uint32_t frequency_hz = kDefaultFrequency) const   = 0;
  virtual void SetDutyCycle(float duty_cycle) const      = 0;
  virtual float GetDutyCycle() const                     = 0;
  virtual void SetFrequency(uint32_t frequency_hz) const = 0;
};
}  // namespace sjsu

#pragma once

#include <cstdint>

#include "utility/status.hpp"

namespace sjsu
{
class Pwm
{
 public:
  // ==============================
  // Interface Defintions
  // ==============================

  static constexpr uint32_t kDefaultFrequency = 1'000;

  // ==============================
  // Interface Methods
  // ==============================

  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  ///
  /// @param frequency_hz - starting frequency of PWM waveform
  virtual Status Initialize(
      uint32_t frequency_hz = kDefaultFrequency) const = 0;
  /// Set output pwm pin to the following duty cycle.
  ///
  /// @param duty_cycle - duty cycle precent from 0 to 1
  virtual void SetDutyCycle(float duty_cycle) const = 0;
  /// @return current duty cycle from hardware
  virtual float GetDutyCycle() const = 0;
  /// Set PWM waveform frequency
  ///
  /// @param frequency_hz - frequency to set the PWM waveform. Cannot be set
  ///        higher than the peripheral frequency of the board.
  virtual void SetFrequency(uint32_t frequency_hz) const = 0;
};
}  // namespace sjsu

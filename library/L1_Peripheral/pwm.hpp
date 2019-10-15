#pragma once

#include <cstdint>

#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// An abstract interface for hardware that can generate Pulse Width Modulation
/// (PWM) waveforms.
class Pwm
{
 public:
  // ==============================
  // Interface Methods
  // ==============================

  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  ///
  /// @param frequency - starting frequency of PWM waveform
  virtual Status Initialize(units::frequency::hertz_t frequency) const = 0;
  /// Set output pwm pin to the following duty cycle.
  ///
  /// @param duty_cycle - duty cycle precent from 0 to 1
  virtual void SetDutyCycle(float duty_cycle) const = 0;
  /// @return current duty cycle from hardware
  virtual float GetDutyCycle() const = 0;
  /// Set PWM waveform frequency
  ///
  /// @param frequency - frequency to set the PWM waveform. Cannot be set
  ///        higher than the peripheral frequency of the board.
  virtual void SetFrequency(units::frequency::hertz_t frequency) const = 0;
};
}  // namespace sjsu

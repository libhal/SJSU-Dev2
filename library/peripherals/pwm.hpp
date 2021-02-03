#pragma once

#include <cstdint>

#include "inactive.hpp"
#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/math/units.hpp"

namespace sjsu
{
/// Generic settings for a standard PWM peripheral
struct PwmSettings_t
{
  /// The frequency of the PWM square wave
  units::frequency::hertz_t frequency = 1_kHz;
};

/// An abstract interface for hardware that can generate Pulse Width Modulation
/// (PWM) waveforms.
///
/// @ingroup l1_peripheral
class Pwm : public Module<PwmSettings_t>
{
 public:
  /// Set the duty cycle of the waveform
  ///
  /// @param duty_cycle - duty cycle precent from 0.0 to 1.0. Where 0.5 would
  ///        be a 50% duty cycle meaning that the pulse will be on for 50% of
  ///        the time and off for the other 50%. Values above or below 0 and 1
  ///        will simply be clamped.
  virtual void SetDutyCycle(float duty_cycle) = 0;

  /// Get the duty cycle percentage
  ///
  /// @return duty cycle as a percentage from 0.0 to 1.0.
  virtual float GetDutyCycle() = 0;
};

/// Template specialization that generates an inactive sjsu::Pwm.
template <>
inline sjsu::Pwm & GetInactive<sjsu::Pwm>()
{
  class InactivePwm : public sjsu::Pwm
  {
   public:
    void ModuleInitialize() override {}
    void SetDutyCycle(float) override {}
    float GetDutyCycle() override
    {
      return 0.0;
    }
  };

  static InactivePwm inactive;
  return inactive;
}
}  // namespace sjsu

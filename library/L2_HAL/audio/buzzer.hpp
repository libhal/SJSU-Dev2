#pragma once

#include <cstdint>

#include "L1_Peripheral/pwm.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// A controller for small speakers and piezoelectric buzzer.
///
/// Usage:
/// ```
/// sjsu::Pwm & buzzer_pwm = GetBuzzerPwm();
/// sjsu::Buzzer buzzer(buzzer_pwm);
/// buzzer.Initializer();
/// // Set buzzer to 550 Hz @ 75% volume.
/// buzzer.Beep(550_Hz, 0.75f);
/// ```
class Buzzer
{
 public:
  /// Initialize Buzzer with a pwm signal.
  explicit constexpr Buzzer(sjsu::Pwm & pwm)
      : pwm_(pwm)
  {
  }
  /// Initialize Buzzer hardware.
  void Initialize()
  {
    pwm_.Initialize(500_Hz);
    Stop();
  }
  /// Turn off buzzer.
  void Stop()
  {
    pwm_.SetDutyCycle(0.0f);
  }
  /// Turn on buzzer at a specific frequency and volume.
  ///
  /// @param frequency - The frequency to set the buzzer to.
  /// @param volume - percent output power from 0.0f to 1.0f.
  void Beep(units::frequency::hertz_t frequency = 500_Hz, float volume = 1.0f)
  {
    pwm_.SetFrequency(frequency);
    // NOTE: Since the PWM is at its loudest at 50% duty cycle, the maximum PWM
    // is divided by 2.
    pwm_.SetDutyCycle(volume / 2);
  }
  /// @return gets the current running volume of the device.
  float GetVolume()
  {
    return pwm_.GetDutyCycle() * 2;
  }
 private:
  Pwm & pwm_;
};
}  // namespace sjsu

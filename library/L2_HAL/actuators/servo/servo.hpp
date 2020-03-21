#pragma once

#include <cstdint>
#include <cstdio>

#include "L1_Peripheral/pwm.hpp"
#include "utility/map.hpp"

namespace sjsu
{
/// RC servo controller that can control servos or other systems that can
/// respond to such signals.
class Servo
{
 public:
  /// Typical Frequency of a generic hobby RC servo.
  static constexpr units::frequency::hertz_t kDefaultFrequency = 50_Hz;
  /// Typical Period of a generic hobby RC servo.
  static constexpr std::chrono::microseconds kDefaultPeriod = 20'000us;
  /// Typical Min Angle of a generic hobby RC servo.
  static constexpr units::angle::degree_t kDefaultMinAngle = 0_deg;
  /// Typical Max Angle of a generic hobby RC servo.
  static constexpr units::angle::degree_t kDefaultMaxAngle = 90_deg;
  /// Typical Min Pulse of a generic hobby RC servo.
  static constexpr std::chrono::microseconds kDefaultMinPulse = 1000us;
  /// Typical Max Pulse of a generic hobby RC servo.
  static constexpr std::chrono::microseconds kDefaultMaxPulse = 2000us;

  /// Construct the servo object
  ///
  /// @param pwm - pwm peripheral to use to generate the PWM signal
  explicit constexpr Servo(const sjsu::Pwm & pwm)
      : servo_pwm_(pwm),
        waveform_period_(kDefaultPeriod),
        pulse_lower_bound_(kDefaultMinPulse),
        pulse_upper_bound_(kDefaultMaxPulse),
        min_angle_(kDefaultMinAngle),
        max_angle_(kDefaultMaxAngle)
  {
  }

  /// Initialize PWM peripheral and setup device to support the required
  /// frequency.
  ///
  /// @param frequency -  the frequency to operate the pulses of the servo.
  virtual void Initialize(
      units::frequency::hertz_t frequency = kDefaultFrequency)
  {
    servo_pwm_.Initialize(frequency);
    SetFrequency(frequency);
  }

  /// Set the RC servo signal's operating frequency.
  ///
  /// @param frequency - the frequency to operate the pulses of the servo.
  virtual void SetFrequency(
      units::frequency::hertz_t frequency = kDefaultFrequency)
  {
    servo_pwm_.SetFrequency(frequency);
    waveform_period_ =
        std::chrono::microseconds((1_MHz / frequency).to<uint32_t>());
  }

  /// Sets the minimum and maximum pulse width lengths that the class will use
  /// to clamp its pulse width output when using SetAngle.
  ///
  /// @param lower - the minimum microseconds pulse that the servo can handle.
  /// @param upper - the maximum microseconds pulse that the servo can handle.
  virtual void SetPulseBounds(std::chrono::microseconds lower,
                              std::chrono::microseconds upper)
  {
    pulse_lower_bound_ = lower;
    pulse_upper_bound_ = upper;
  }

  /// Sets your angle bounds that maps angles to microseconds when using
  /// SetAngle.
  ///
  /// @param min_angle - The minimum angle to limit the servo is bound to.
  /// @param max_angle - The maximum angle to limit the servo is bound to.
  virtual void SetAngleBounds(units::angle::degree_t min_angle,
                              units::angle::degree_t max_angle)
  {
    min_angle_ = min_angle;
    max_angle_ = max_angle;
  }

  /// Set the pulse width in microsecond of the RC servo signal directly.
  ///
  /// @param pulse_width - how long the high side of the RC pulse should be.
  virtual void SetPulseWidthInMicroseconds(
      std::chrono::microseconds pulse_width)
  {
    servo_pwm_.SetDutyCycle(static_cast<float>(pulse_width.count()) /
                            static_cast<float>(waveform_period_.count()));
  }

  /// Should only be used after pulse bounds and angle bounds have been set.
  ///
  /// @param angle - angle to position the servo to.
  virtual void SetAngle(units::angle::degree_t angle)
  {
    float pulse_width = Map(angle.to<float>(),
                            min_angle_.to<float>(),
                            max_angle_.to<float>(),
                            static_cast<float>(pulse_lower_bound_.count()),
                            static_cast<float>(pulse_upper_bound_.count()));
    SetPulseWidthInMicroseconds(
        std::chrono::microseconds(static_cast<uint32_t>(pulse_width)));
  }

 private:
  const Pwm & servo_pwm_;
  std::chrono::microseconds waveform_period_;
  std::chrono::microseconds pulse_lower_bound_;
  std::chrono::microseconds pulse_upper_bound_;
  units::angle::degree_t min_angle_;
  units::angle::degree_t max_angle_;
};
}  // namespace sjsu

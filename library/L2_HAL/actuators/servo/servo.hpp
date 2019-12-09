#pragma once

#include <cstdint>
#include <cstdio>

#include "L1_Peripheral/pwm.hpp"
#include "utility/map.hpp"

namespace sjsu
{
class Servo
{
 public:
  static constexpr units::frequency::hertz_t kDefaultFrequency = 50_Hz;
  static constexpr std::chrono::microseconds kDefaultPeriod    = 20'000us;
  static constexpr units::angle::degree_t kDefaultMinAngle     = 0_deg;
  static constexpr units::angle::degree_t kDefaultMaxAngle     = 90_deg;
  static constexpr std::chrono::microseconds kDefaultMinPulse  = 1000us;
  static constexpr std::chrono::microseconds kDefaultMaxPulse  = 2000us;

  explicit constexpr Servo(const sjsu::Pwm & pwm)
      : servo_pwm_(pwm),
        waveform_period_(kDefaultPeriod),
        pulse_lower_bound_(kDefaultMinPulse),
        pulse_upper_bound_(kDefaultMaxPulse),
        min_angle_(kDefaultMinAngle),
        max_angle_(kDefaultMaxAngle)
  {
  }

  virtual void Initialize(
      units::frequency::hertz_t frequency = kDefaultFrequency)
  {
    servo_pwm_.Initialize(frequency);
    SetFrequency(frequency);
  }

  virtual void SetFrequency(
      units::frequency::hertz_t frequency = kDefaultFrequency)
  {
    servo_pwm_.SetFrequency(frequency);
    waveform_period_ =
        std::chrono::microseconds((1_MHz / frequency).to<uint32_t>());
  }

  // Sets the minimum and maximum pulse width lengths that the class will use
  // to clamp its pulse width output when using SetAngle.
  virtual void SetPulseBounds(std::chrono::microseconds lower,
                              std::chrono::microseconds upper)
  {
    pulse_lower_bound_ = lower;
    pulse_upper_bound_ = upper;
  }

  // Sets your angle bounds that maps angles to microseconds when using
  // SetAngle.
  virtual void SetAngleBounds(units::angle::degree_t min_angle,
                              units::angle::degree_t max_angle)
  {
    min_angle_ = min_angle;
    max_angle_ = max_angle;
  }

  virtual void SetPulseWidthInMicroseconds(
      std::chrono::microseconds pulse_width)
  {
    servo_pwm_.SetDutyCycle(static_cast<float>(pulse_width.count()) /
                            static_cast<float>(waveform_period_.count()));
  }

  // Should only be used after pulse bounds and angle bounds have been set.
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

#pragma once

#include <cstdint>
#include <cstdio>

#include "peripherals/pwm.hpp"
#include "module.hpp"
#include "utility/math/map.hpp"

namespace sjsu
{
/// Settings for Servos that use RC servo PWM signals
struct RCServoSettings_t
{
  /// Typical Frequency of a generic hobby RC servo.
  units::frequency::hertz_t frequency = 50_Hz;
  /// Typical Min Angle of a generic hobby RC servo.
  units::angle::degree_t min_angle = 0_deg;
  /// Typical Max Angle of a generic hobby RC servo.
  units::angle::degree_t max_angle = 90_deg;
  /// Typical Min Pulse of a generic hobby RC servo.
  std::chrono::microseconds min_pulse = 1000us;
  /// Typical Max Pulse of a generic hobby RC servo.
  std::chrono::microseconds max_pulse = 2000us;

  /// Sets the minimum and maximum pulse width lengths that the class will use
  /// to clamp its pulse width output when using SetAngle.
  ///
  /// @param new_min_pulse - the minimum pulse width that the servo can handle.
  /// @param new_max_pulse - the maximum pulse width that the servo can handle.
  auto & PulseBounds(std::chrono::microseconds new_min_pulse,
                     std::chrono::microseconds new_max_pulse)
  {
    min_pulse = new_min_pulse;
    max_pulse = new_max_pulse;
    return *this;
  }

  /// Sets your angle bounds that maps angles to microseconds when using
  /// SetAngle.
  ///
  /// @param new_min_angle - The minimum angle to limit the servo to.
  /// @param new_max_angle - The maximum angle to limit the servo to.
  auto & AngleBounds(units::angle::degree_t new_min_angle,
                     units::angle::degree_t new_max_angle)
  {
    min_angle = new_min_angle;
    max_angle = new_max_angle;
    return *this;
  }
};

/// RC servo controller that can control servos or other systems that can
/// respond to such signals.
class Servo : public Module<RCServoSettings_t>
{
 public:
  /// Construct the servo object
  ///
  /// @param pwm - pwm peripheral to use to generate the PWM signal
  explicit constexpr Servo(sjsu::Pwm & pwm) : servo_pwm_(pwm) {}

  void ModuleInitialize() override
  {
    servo_pwm_.settings.frequency = settings.frequency;
    servo_pwm_.Initialize();
  }

  /// Set the pulse width in microsecond of the RC servo signal directly.
  ///
  /// @param pulse_width - how long the high side of the RC pulse should be.
  void SetPulseWidthInMicroseconds(std::chrono::microseconds pulse_width)
  {
    const auto kPulseWidth = static_cast<float>(pulse_width.count());
    const auto kWaveformPeriod =
        FrequencyToMicrosecondsValue(CurrentSettings().frequency);

    servo_pwm_.SetDutyCycle(kPulseWidth / kWaveformPeriod);
  }

  /// Should only be used after pulse bounds and angle bounds have been set.
  ///
  /// @param angle - angle to position the servo to.
  void SetAngle(units::angle::degree_t angle)
  {
    auto local_settings  = CurrentSettings();
    const auto kMinPulse = static_cast<float>(local_settings.min_pulse.count());
    const auto kMaxPulse = static_cast<float>(local_settings.max_pulse.count());

    const auto kClampedAngle =
        std::clamp(angle, local_settings.min_angle, local_settings.max_angle);

    float pulse_width = Map(kClampedAngle.to<float>(),
                            local_settings.min_angle.to<float>(),
                            local_settings.max_angle.to<float>(),
                            kMinPulse,
                            kMaxPulse);
    SetPulseWidthInMicroseconds(
        std::chrono::microseconds(static_cast<uint32_t>(pulse_width)));
  }

 private:
  constexpr float FrequencyToMicrosecondsValue(
      units::frequency::hertz_t frequency)
  {
    return (1_MHz / frequency).to<float>();
  }

  Pwm & servo_pwm_;
};
}  // namespace sjsu

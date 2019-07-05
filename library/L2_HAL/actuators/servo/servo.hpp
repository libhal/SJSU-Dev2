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
    static constexpr uint32_t kDefaultFrequency = 50;
    static constexpr float kDefaultMinAngle = 0;
    static constexpr float kDefaultMaxAngle = 90;
    static constexpr float kDefaultMinPulse = 1000;
    static constexpr float kDefaultMaxPulse = 2000;

    explicit constexpr Servo(const sjsu::Pwm & pwm)
      : servo_pwm_(pwm),
        max_pulse_length_us_(0),
        pulse_lower_bound_(kDefaultMinPulse),
        pulse_upper_bound_(kDefaultMaxPulse),
        min_angle_(kDefaultMinAngle),
        max_angle_(kDefaultMaxAngle)
    {
    }

    virtual void Initialize(uint32_t frequency = kDefaultFrequency)
    {
      servo_pwm_.Initialize(frequency);
      max_pulse_length_us_ = (1 / static_cast<float>(frequency)) * 1'000'000;
    }

    virtual void SetFrequency(uint32_t frequency = kDefaultFrequency)
    {
      servo_pwm_.SetFrequency(frequency);
      max_pulse_length_us_ = (1 / static_cast<float>(frequency)) * 1'000'000;
    }

    // Sets the minimum and maximum pulse width lengths that the class will
    // use to clamp its pulse width output when using SetAngle.
    virtual void SetPulseBounds(float lower, float upper)
    {
      pulse_lower_bound_ = lower;
      pulse_upper_bound_ = upper;
    }

    // Sets your angle bounds that maps angles to microseconds when
    // using SetAngle
    virtual void SetAngleBounds(float min_angle, float max_angle)
    {
      min_angle_ = min_angle;
      max_angle_ = max_angle;
    }

    virtual void SetPulseWidthInMicroseconds(float pulse_length_us)
    {
      servo_pwm_.SetDutyCycle(pulse_length_us / max_pulse_length_us_);
    }

    // Should only be used after pulse bounds and angle bounds have been set.
    virtual void SetAngle(float angle)
    {
      float pulse_length_us = Map(angle,
                                  min_angle_,
                                  max_angle_,
                                  pulse_lower_bound_,
                                  pulse_upper_bound_);
      SetPulseWidthInMicroseconds(pulse_length_us);
    }

 private:
    const Pwm & servo_pwm_;
    float max_pulse_length_us_;
    float pulse_lower_bound_;
    float pulse_upper_bound_;
    float min_angle_;
    float max_angle_;
};
}  // namespace sjsu

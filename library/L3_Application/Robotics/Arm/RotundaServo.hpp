#pragma once
#include "L2_HAL/actuators/servo/servo.hpp"

namespace sjsu
{
namespace robotics
{
class RotundaServo : public Servo
{
 public:
  explicit constexpr RotundaServo(const Pwm & pwm) : Servo(pwm) {}

  void Initialize(units::frequency::hertz_t frequency = 50_Hz) override
  {
    Servo::Initialize(frequency);
    Servo::SetAngleBounds(-1540_deg, 1540_deg);
    Servo::SetPulseBounds(500us, 2500us);
  }
};
}  // namespace robotics
}  // namespace sjsu

// set all three wheel angle
// set all three wheel speed
// stop errthang
// initialize
// constructor

#pragma once

#include "L3_Application/Robotics/Drive/HubMotor.hpp"
#include "L3_Application/Robotics/Common/ChiHaiServo.hpp"

namespace sjsu
{
namespace robotics
{
class DriveController
{
 public:
  explicit constexpr DriveController(sjsu::robotics::HubMotor & wheel_a,
                                     sjsu::robotics::HubMotor & wheel_b,
                                     sjsu::robotics::HubMotor & wheel_c,
                                     sjsu::robotics::ChiHaiServo & steering_a,
                                     sjsu::robotics::ChiHaiServo & steering_b,
                                     sjsu::robotics::ChiHaiServo & steering_c)
      : wheel_a_(wheel_a),
        wheel_b_(wheel_b),
        wheel_c_(wheel_c),
        steering_a_(steering_a),
        steering_b_(steering_b),
        steering_c_(steering_c)
  {
  }
  void Initialize()
  {
    wheel_a_.Initialize();
    wheel_b_.Initialize();
    wheel_c_.Initialize();
    steering_a_.Initialize();
    steering_b_.Initialize();
    steering_c_.Initialize();
  }
  void Stop()
  {
    steering_a_.Stop();
    steering_b_.Stop();
    steering_c_.Stop();
    wheel_a_.Stop();
    wheel_b_.Stop();
    wheel_c_.Stop();
  }
  void SetWheelASpeedAndDirection(float duty_cycle,
                                  sjsu::robotics::HubMotor::Direction direction)
  {
    wheel_a_.SetSpeedAndDirection(duty_cycle, direction);
  }
  void SetWheelBSpeedAndDirection(float duty_cycle,
                                  sjsu::robotics::HubMotor::Direction direction)
  {
    wheel_a_.SetSpeedAndDirection(duty_cycle, direction);
  }
  void SetWheelCSpeedAndDirection(float duty_cycle,
                                  sjsu::robotics::HubMotor::Direction direction)
  {
    wheel_a_.SetSpeedAndDirection(duty_cycle, direction);
  }
  void SetWheelAAngle(units::angle::degree_t angle)
  {
    steering_a_.SetAngle(angle);
  }
  void SetWheelBAngle(units::angle::degree_t angle)
  {
    steering_b_.SetAngle(angle);
  }
  void SetWheelCAngle(units::angle::degree_t angle)
  {
    steering_c_.SetAngle(angle);
  }

 private:
  sjsu::robotics::HubMotor & wheel_a_;
  sjsu::robotics::HubMotor & wheel_b_;
  sjsu::robotics::HubMotor & wheel_c_;
  sjsu::robotics::ChiHaiServo & steering_a_;
  sjsu::robotics::ChiHaiServo & steering_b_;
  sjsu::robotics::ChiHaiServo & steering_c_;
};
}  // namespace robotics
}  // namespace sjsu

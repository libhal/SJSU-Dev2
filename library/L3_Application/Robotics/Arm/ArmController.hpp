#pragma once

#include "utility/units.hpp"
#include "L3_Application/Robotics/Common/ChiHaiServo.hpp"
#include "L3_Application/Robotics/Arm/RotundaServo.hpp"

namespace sjsu
{
namespace robotics
{
class ArmController
{
 public:
  explicit constexpr ArmController(sjsu::robotics::ChiHaiServo & shoulder,
                                   sjsu::robotics::ChiHaiServo & wrist,
                                   sjsu::robotics::RotundaServo & rotunda)
      : shoulder_(shoulder), wrist_(wrist), rotunda_(rotunda)
  {
  }

  void Initialize()
  {
    shoulder_.Initialize();
    wrist_.Initialize();
    rotunda_.Initialize();
  }

  void SetShoulderAngle(units::angle::degree_t angle)
  {
    shoulder_.SetAngle(angle);
  }

  void SetWristAngle(units::angle::degree_t angle)
  {
    wrist_.SetAngle(angle);
  }

  void SetRotundaAngle(units::angle::degree_t angle)
  {
    rotunda_.SetAngle(angle);
  }

  void Stop() {
    shoulder_.Stop();
    wrist_.Stop();
  }

 private:
  sjsu::robotics::ChiHaiServo & shoulder_;
  sjsu::robotics::ChiHaiServo & wrist_;
  sjsu::robotics::RotundaServo & rotunda_;
  /* data */
};

}  // namespace robotics
}  // namespace sjsu
// ChiHaiServo.hpp World

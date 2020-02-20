#pragma once

#include "utility/log.hpp"
#include "utility/units.hpp"
#include "L1_Peripheral/pwm.hpp"
#include "L1_Peripheral/gpio.hpp"
#include "L3_Application/Robotics/Common/MagneticEncoder.hpp"

namespace sjsu
{
namespace robotics
{
static constexpr float kPwmHigh = 1.0f;
static constexpr float kPwmLow  = 1.0f;
// need a pwm phase, and gpio enable
class Drv8801
{
 public:
  explicit constexpr Drv8801(sjsu::Pwm & enable,
                             sjsu::Gpio & direction_pin,
                             sjsu::Gpio & mode_pin)
      : enable_(enable), direction_pin_(direction_pin), mode_pin_(mode_pin)
  {
  }
  void Initialize()
  {
    enable_.Initialize(50_Hz);
    direction_pin_.SetAsOutput();
    mode_pin_.SetAsOutput();
    mode_pin_.SetLow();
  }
  void TurnForward()
  {
    LOG_INFO("Direction pin high, PWM high");
    direction_pin_.SetHigh();
    enable_.SetDutyCycle(kPwmHigh);
  }
  void TurnBackward()
  {
    LOG_INFO("Direction pin low, PWM high");
    direction_pin_.SetLow();
    enable_.SetDutyCycle(kPwmHigh);
  }
  void Stop()
  {
    LOG_INFO("PWM low");
    enable_.SetDutyCycle(kPwmLow);
  }

 private:
  // Pwm pin to enable the motor to move. When it is 0 the motor stops.
  const sjsu::Pwm & enable_;
  // Gpio pin to control the movement. If it is high the motor goes forward, if
  // it is low the motor goes backward
  const sjsu::Gpio & direction_pin_;
  const sjsu::Gpio & mode_pin_;
};
}  // namespace robotics
}  // namespace sjsu

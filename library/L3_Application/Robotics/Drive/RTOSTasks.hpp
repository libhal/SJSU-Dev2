#include <cstdint>
#include "FreeRTOS.h"

#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L1_Peripheral/lpc40xx/pwm.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L3_Application/Robotics/Common/Params.hpp"
#include "L3_Application/Robotics/Common/ChiHaiServo.hpp"
#include "L3_Application/Robotics/Common/MagneticEncoder.hpp"
#include "L3_Application/Robotics/Common/Drv8801.hpp"
#include "L3_Application/Robotics/Drive/HubMotor.hpp"
#include "L3_Application/Robotics/Drive/DriveController.hpp"
#include "utility/log.hpp"
#include "utility/rtos.hpp"

void vMoveTask(void * pvParameters)
{
  sjsu::robotics::ParamsStruct * params =
      static_cast<sjsu::robotics::ParamsStruct *>(pvParameters);

  sjsu::lpc40xx::Gpio hub_direction_a(0, 0);
  sjsu::lpc40xx::Gpio hub_direction_b(0, 1);
  sjsu::lpc40xx::Gpio hub_direction_c(0, 2);

  sjsu::lpc40xx::Gpio servo_direction_a(1, 0);
  sjsu::lpc40xx::Gpio servo_direction_b(1, 1);
  sjsu::lpc40xx::Gpio servo_direction_c(1, 2);

  sjsu::lpc40xx::Pwm hub_pwm_a(sjsu::lpc40xx::Pwm::Channel::kPwm0);
  sjsu::lpc40xx::Pwm hub_pwm_b(sjsu::lpc40xx::Pwm::Channel::kPwm1);
  sjsu::lpc40xx::Pwm hub_pwm_c(sjsu::lpc40xx::Pwm::Channel::kPwm2);

  sjsu::lpc40xx::Pwm servo_pwm_a(sjsu::lpc40xx::Pwm::Channel::kPwm3);
  sjsu::lpc40xx::Pwm servo_pwm_b(sjsu::lpc40xx::Pwm::Channel::kPwm4);
  sjsu::lpc40xx::Pwm servo_pwm_c(sjsu::lpc40xx::Pwm::Channel::kPwm5);

  sjsu::lpc40xx::I2c encoder_i2c_a(sjsu::lpc40xx::I2c::Bus::kI2c0);
  sjsu::lpc40xx::I2c encoder_i2c_b(sjsu::lpc40xx::I2c::Bus::kI2c1);
  sjsu::lpc40xx::I2c encoder_i2c_c(sjsu::lpc40xx::I2c::Bus::kI2c2);

  sjsu::robotics::HubMotor wheel_a(hub_pwm_a, hub_direction_a);
  sjsu::robotics::HubMotor wheel_b(hub_pwm_b, hub_direction_b);
  sjsu::robotics::HubMotor wheel_c(hub_pwm_c, hub_direction_c);

  sjsu::robotics::Drv8801 drv_a(servo_pwm_a, servo_direction_a);
  sjsu::robotics::Drv8801 drv_b(servo_pwm_b, servo_direction_b);
  sjsu::robotics::Drv8801 drv_c(servo_pwm_c, servo_direction_c);

  sjsu::robotics::MagneticEncoder magnetic_encoder_a(encoder_i2c_a);
  sjsu::robotics::MagneticEncoder magnetic_encoder_b(encoder_i2c_b);
  sjsu::robotics::MagneticEncoder magnetic_encoder_c(encoder_i2c_c);

  sjsu::robotics::ChiHaiServo servo_a(magnetic_encoder_a, drv_a);
  sjsu::robotics::ChiHaiServo servo_b(magnetic_encoder_b, drv_b);
  sjsu::robotics::ChiHaiServo servo_c(magnetic_encoder_c, drv_c);

  sjsu::robotics::DriveController drive(
      wheel_a, wheel_b, wheel_c, servo_a, servo_b, servo_c);

  // todo: add drive constants for direction and wheel type
  while (1)
  {
    sjsu::robotics::HubMotor::Direction wheel_direction =
        params->drive_reverse ? sjsu::robotics::HubMotor::Direction::kBackward
                              : sjsu::robotics::HubMotor::Direction::kForward;
    if (params->wheel_a)
    {
      drive.SetWheelASpeedAndDirection(params->wheel_speed, wheel_direction);
    }
    else if (params->wheel_b)
    {
      drive.SetWheelBSpeedAndDirection(params->wheel_speed, wheel_direction);
    }
    else if (params->wheel_c)
    {
      drive.SetWheelCSpeedAndDirection(params->wheel_speed, wheel_direction);
    }
    else
    {
      drive.Stop();
    }
    LOG_INFO("wheel_a: %d, arm_angle: %f", params->wheel_a, params->arm_angle);
    vTaskDelay(100);
  }
}

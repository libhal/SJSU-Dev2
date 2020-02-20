#include <cstdint>
#include "FreeRTOS.h"

#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L1_Peripheral/lpc40xx/pwm.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "utility/log.hpp"
#include "utility/rtos.hpp"
#include "L3_Application/Robotics/Common/Params.hpp"
#include "L3_Application/Robotics/Drive/HubMotor.hpp"

// this is copied and pasted from drive rtos tasks. modify on thursday (2/20)
void vMoveTask(void * pvParameters)
{
  sjsu::robotics::ParamsStruct * params = (sjsu::robotics::ParamsStruct *)pvParameters;
  sjsu::lpc40xx::Gpio direction_a(0, 1);
  sjsu::lpc40xx::Gpio direction_b(0, 2);
  sjsu::lpc40xx::Gpio direction_c(0, 3);
  sjsu::lpc40xx::Pwm pwm_a(sjsu::lpc40xx::Pwm::Channel::kPwm0);
  sjsu::lpc40xx::Pwm pwm_b(sjsu::lpc40xx::Pwm::Channel::kPwm1);
  sjsu::lpc40xx::Pwm pwm_c(sjsu::lpc40xx::Pwm::Channel::kPwm2);
  sjsu::robotics::HubMotor wheel_a(pwm_a, direction_a);
  sjsu::robotics::HubMotor wheel_b(pwm_b, direction_b);
  sjsu::robotics::HubMotor wheel_c(pwm_c, direction_c);

  while (1)
  {
    LOG_INFO("wheel_a: %d, arm_angle: %f", params->wheel_a, params->arm_angle);
    vTaskDelay(100);
  }
}

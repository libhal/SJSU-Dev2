#pragma once

#include "L1_Peripheral/lpc17xx/pin.hpp"
#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L1_Peripheral/lpc40xx/pwm.hpp"

namespace sjsu
{
namespace lpc17xx
{
// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Pwm;
/// Structure used as a namespace for predefined Channel definitions
struct PwmChannel  // NOLINT
{
 private:
  inline static lpc17xx::Pin pwm_pin0 = lpc17xx::Pin(2, 0);
  inline static lpc17xx::Pin pwm_pin1 = lpc17xx::Pin(2, 1);
  inline static lpc17xx::Pin pwm_pin2 = lpc17xx::Pin(2, 2);
  inline static lpc17xx::Pin pwm_pin3 = lpc17xx::Pin(2, 3);
  inline static lpc17xx::Pin pwm_pin4 = lpc17xx::Pin(2, 4);
  inline static lpc17xx::Pin pwm_pin5 = lpc17xx::Pin(2, 5);

 public:
  /// Definition of the PWM 1 peripheral.
  inline static const lpc40xx::Pwm::Peripheral_t kPwm1Peripheral = {
    .registers = reinterpret_cast<lpc40xx::LPC_PWM_TypeDef *>(LPC_PWM1),
    .id        = SystemController::Peripherals::kPwm1,
  };
  /// Definition for channel 0 of PWM peripheral 1.
  inline static const lpc40xx::Pwm::Channel_t kPwm0 = {
    .peripheral        = kPwm1Peripheral,
    .pin               = pwm_pin0,
    .channel           = 1,
    .pin_function_code = 0b01,
  };
  /// Definition for channel 1 of PWM peripheral 1.
  inline static const lpc40xx::Pwm::Channel_t kPwm1 = {
    .peripheral        = kPwm1Peripheral,
    .pin               = pwm_pin1,
    .channel           = 2,
    .pin_function_code = 0b01,
  };
  /// Definition for channel 2 of PWM peripheral 1.
  inline static const lpc40xx::Pwm::Channel_t kPwm2 = {
    .peripheral        = kPwm1Peripheral,
    .pin               = pwm_pin2,
    .channel           = 3,
    .pin_function_code = 0b01,
  };
  /// Definition for channel 3 of PWM peripheral 1.
  inline static const lpc40xx::Pwm::Channel_t kPwm3 = {
    .peripheral        = kPwm1Peripheral,
    .pin               = pwm_pin3,
    .channel           = 4,
    .pin_function_code = 0b01,
  };
  /// Definition for channel 4 of PWM peripheral 1.
  inline static const lpc40xx::Pwm::Channel_t kPwm4 = {
    .peripheral        = kPwm1Peripheral,
    .pin               = pwm_pin4,
    .channel           = 5,
    .pin_function_code = 0b01,
  };
  /// Definition for channel 5 of PWM peripheral 1.
  inline static const lpc40xx::Pwm::Channel_t kPwm5 = {
    .peripheral        = kPwm1Peripheral,
    .pin               = pwm_pin5,
    .channel           = 6,
    .pin_function_code = 0b01,
  };
};
}  // namespace lpc17xx
}  // namespace sjsu

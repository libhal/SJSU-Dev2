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

struct PwmChannel  // NOLINT
{
 private:
  inline static const lpc40xx::Pwm::Peripheral_t kPwm1PeripheralCommon = {
    .registers   = reinterpret_cast<lpc40xx::LPC_PWM_TypeDef *>(LPC_PWM1),
    .power_on_id = SystemController::Peripherals::kPwm1,
  };
  inline static const Pin kPwmPin0 = Pin::CreatePin<2, 0>();
  inline static const Pin kPwmPin1 = Pin::CreatePin<2, 1>();
  inline static const Pin kPwmPin2 = Pin::CreatePin<2, 2>();
  inline static const Pin kPwmPin3 = Pin::CreatePin<2, 3>();
  inline static const Pin kPwmPin4 = Pin::CreatePin<2, 4>();
  inline static const Pin kPwmPin5 = Pin::CreatePin<2, 5>();

 public:
  inline static const lpc40xx::Pwm::Channel_t kPwm0 = {
    .peripheral      = kPwm1PeripheralCommon,
    .pin             = kPwmPin0,
    .channel         = 1,
    .pin_function_id = 0b01,
  };
  inline static const lpc40xx::Pwm::Channel_t kPwm1 = {
    .peripheral      = kPwm1PeripheralCommon,
    .pin             = kPwmPin1,
    .channel         = 2,
    .pin_function_id = 0b01,
  };
  inline static const lpc40xx::Pwm::Channel_t kPwm2 = {
    .peripheral      = kPwm1PeripheralCommon,
    .pin             = kPwmPin2,
    .channel         = 3,
    .pin_function_id = 0b01,
  };
  inline static const lpc40xx::Pwm::Channel_t kPwm3 = {
    .peripheral      = kPwm1PeripheralCommon,
    .pin             = kPwmPin3,
    .channel         = 4,
    .pin_function_id = 0b01,
  };
  inline static const lpc40xx::Pwm::Channel_t kPwm4 = {
    .peripheral      = kPwm1PeripheralCommon,
    .pin             = kPwmPin4,
    .channel         = 5,
    .pin_function_id = 0b01,
  };
  inline static const lpc40xx::Pwm::Channel_t kPwm5 = {
    .peripheral      = kPwm1PeripheralCommon,
    .pin             = kPwmPin5,
    .channel         = 6,
    .pin_function_id = 0b01,
  };
};
}  // namespace lpc17xx
}  // namespace sjsu

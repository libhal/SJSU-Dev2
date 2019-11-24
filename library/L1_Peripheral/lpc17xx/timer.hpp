#pragma once

#include <functional>

#include "L1_Peripheral/lpc17xx/pin.hpp"
#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L1_Peripheral/lpc40xx/timer.hpp"

namespace sjsu
{
namespace lpc17xx
{
// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Timer;

struct TimerChannel  // NOLINT
{
 private:
  inline static InterruptCallback timer0_callback                     = nullptr;
  inline static const lpc40xx::Timer::ChannelPartial_t kTimerPartial0 = {
    .timer_register = reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(LPC_TIM0),
    .power_id       = SystemController::Peripherals::kTimer0,
    .irq            = static_cast<lpc40xx::IRQn>(IRQn::TIMER0_IRQn),
    .user_callback  = &timer0_callback,
  };
  inline static InterruptCallback timer1_callback                     = nullptr;
  inline static const lpc40xx::Timer::ChannelPartial_t kTimerPartial1 = {
    .timer_register = reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(LPC_TIM1),
    .power_id       = SystemController::Peripherals::kTimer1,
    .irq            = static_cast<lpc40xx::IRQn>(IRQn::TIMER1_IRQn),
    .user_callback  = &timer1_callback,
  };
  inline static InterruptCallback timer2_callback                     = nullptr;
  inline static const lpc40xx::Timer::ChannelPartial_t kTimerPartial2 = {
    .timer_register = reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(LPC_TIM2),
    .power_id       = SystemController::Peripherals::kTimer2,
    .irq            = static_cast<lpc40xx::IRQn>(IRQn::TIMER2_IRQn),
    .user_callback  = &timer2_callback,
  };
  inline static InterruptCallback timer3_callback                     = nullptr;
  inline static const lpc40xx::Timer::ChannelPartial_t kTimerPartial3 = {
    .timer_register = reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(LPC_TIM3),
    .power_id       = SystemController::Peripherals::kTimer3,
    .irq            = static_cast<lpc40xx::IRQn>(IRQn::TIMER3_IRQn),
    .user_callback  = &timer3_callback,
  };

 public:
  inline static const lpc40xx::Timer::Channel_t kTimer0 = {
    .channel = kTimerPartial0,
    .handler = lpc40xx::Timer::TimerHandler<kTimerPartial0>,
  };
  inline static const lpc40xx::Timer::Channel_t kTimer1 = {
    .channel = kTimerPartial1,
    .handler = lpc40xx::Timer::TimerHandler<kTimerPartial1>,
  };
  inline static const lpc40xx::Timer::Channel_t kTimer2 = {
    .channel = kTimerPartial2,
    .handler = lpc40xx::Timer::TimerHandler<kTimerPartial2>,
  };
  inline static const lpc40xx::Timer::Channel_t kTimer3 = {
    .channel = kTimerPartial3,
    .handler = lpc40xx::Timer::TimerHandler<kTimerPartial3>,
  };
};
}  // namespace lpc17xx
}  // namespace sjsu

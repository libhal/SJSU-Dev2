#pragma once

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
  inline static IsrPointer timer0_isr                                 = nullptr;
  inline static const lpc40xx::Timer::ChannelPartial_t kTimerPartial0 = {
    .timer_register = reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(LPC_TIM0),
    .power_id       = SystemController::Peripherals::kTimer0,
    .irq            = static_cast<lpc40xx::IRQn>(IRQn::TIMER0_IRQn),
    .user_callback  = &timer0_isr,
  };
  inline static IsrPointer timer1_isr                                 = nullptr;
  inline static const lpc40xx::Timer::ChannelPartial_t kTimerPartial1 = {
    .timer_register = reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(LPC_TIM1),
    .power_id       = SystemController::Peripherals::kTimer1,
    .irq            = static_cast<lpc40xx::IRQn>(IRQn::TIMER1_IRQn),
    .user_callback  = &timer1_isr,
  };
  inline static IsrPointer timer2_isr                                 = nullptr;
  inline static const lpc40xx::Timer::ChannelPartial_t kTimerPartial2 = {
    .timer_register = reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(LPC_TIM2),
    .power_id       = SystemController::Peripherals::kTimer2,
    .irq            = static_cast<lpc40xx::IRQn>(IRQn::TIMER2_IRQn),
    .user_callback  = &timer2_isr,
  };
  inline static IsrPointer timer3_isr                                 = nullptr;
  inline static const lpc40xx::Timer::ChannelPartial_t kTimerPartial3 = {
    .timer_register = reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(LPC_TIM3),
    .power_id       = SystemController::Peripherals::kTimer3,
    .irq            = static_cast<lpc40xx::IRQn>(IRQn::TIMER3_IRQn),
    .user_callback  = &timer3_isr,
  };

 public:
  inline static const lpc40xx::Timer::Channel_t kTimer0 = {
    .channel = kTimerPartial0,
    .isr     = lpc40xx::Timer::TimerHandler<kTimerPartial0>,
  };
  inline static const lpc40xx::Timer::Channel_t kTimer1 = {
    .channel = kTimerPartial1,
    .isr     = lpc40xx::Timer::TimerHandler<kTimerPartial1>,
  };
  inline static const lpc40xx::Timer::Channel_t kTimer2 = {
    .channel = kTimerPartial2,
    .isr     = lpc40xx::Timer::TimerHandler<kTimerPartial2>,
  };
  inline static const lpc40xx::Timer::Channel_t kTimer3 = {
    .channel = kTimerPartial3,
    .isr     = lpc40xx::Timer::TimerHandler<kTimerPartial3>,
  };
};
}  // namespace lpc17xx
}  // namespace sjsu

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

/// Namespace containing the available peripherals for the LPC17xx platform
struct TimerPeripheral  // NOLINT
{
  /// Peripheral descriptor for TIMER 0
  inline static const lpc40xx::Timer::Peripheral_t kTimer0 = {
    .peripheral = reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(LPC_TIM0),
    .id         = SystemController::Peripherals::kTimer0,
    .irq        = IRQn::TIMER0_IRQn,
  };
  /// Peripheral descriptor for TIMER 1
  inline static const lpc40xx::Timer::Peripheral_t kTimer1 = {
    .peripheral = reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(LPC_TIM1),
    .id         = SystemController::Peripherals::kTimer1,
    .irq        = IRQn::TIMER1_IRQn,
  };
  /// Peripheral descriptor for TIMER 2
  inline static const lpc40xx::Timer::Peripheral_t kTimer2 = {
    .peripheral = reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(LPC_TIM2),
    .id         = SystemController::Peripherals::kTimer2,
    .irq        = IRQn::TIMER2_IRQn,
  };
  /// Peripheral descriptor for TIMER 3
  inline static const lpc40xx::Timer::Peripheral_t kTimer3 = {
    .peripheral = reinterpret_cast<lpc40xx::LPC_TIM_TypeDef *>(LPC_TIM3),
    .id         = SystemController::Peripherals::kTimer3,
    .irq        = IRQn::TIMER3_IRQn,
  };
};
}  // namespace lpc17xx
}  // namespace sjsu

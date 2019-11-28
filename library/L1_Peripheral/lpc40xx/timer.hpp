#pragma once

#include <cstdint>
#include <limits>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/timer.hpp"
#include "utility/bit.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"
#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
namespace lpc40xx
{
class Timer final : public sjsu::Timer
{
 public:
  struct Peripheral_t
  {
    LPC_TIM_TypeDef * peripheral;
    sjsu::SystemController::PeripheralID id;
    int irq;
  };

  struct Peripheral  // NOLINT
  {
    inline static const Peripheral_t kTimer0 = {
      .peripheral = LPC_TIM0,
      .id         = sjsu::lpc40xx::SystemController::Peripherals::kTimer0,
      .irq        = IRQn::TIMER0_IRQn,
    };

    inline static const Peripheral_t kTimer1 = {
      .peripheral = LPC_TIM1,
      .id         = sjsu::lpc40xx::SystemController::Peripherals::kTimer1,
      .irq        = IRQn::TIMER1_IRQn,
    };

    inline static const Peripheral_t kTimer2 = {
      .peripheral = LPC_TIM2,
      .id         = sjsu::lpc40xx::SystemController::Peripherals::kTimer2,
      .irq        = IRQn::TIMER2_IRQn,
    };

    inline static const Peripheral_t kTimer3 = {
      .peripheral = LPC_TIM3,
      .id         = sjsu::lpc40xx::SystemController::Peripherals::kTimer3,
      .irq        = IRQn::TIMER3_IRQn,
    };
  };

  static constexpr uint8_t kMatchRegisterCount = 4;

  explicit constexpr Timer(const Peripheral_t & timer,
                           const sjsu::SystemController & system_controller =
                               DefaultSystemController())
      : timer_(timer), system_controller_(system_controller)
  {
  }

  Status Initialize(units::frequency::hertz_t frequency,
                    InterruptCallback callback = nullptr,
                    int32_t priority           = -1) const override
  {
    system_controller_.PowerUpPeripheral(timer_.id);
    SJ2_ASSERT_FATAL(
        frequency.to<uint32_t>() != 0,
        "Cannot have zero ticks per microsecond, please choose 1 or more.");
    // Set Prescale register for Prescale Counter to milliseconds
    uint32_t prescaler =
        system_controller_.GetPeripheralFrequency(timer_.id) / frequency;
    timer_.peripheral->PR = prescaler;
    timer_.peripheral->TCR |= (1 << 0);

    // Take the class's address and a copy of the callback for use in the
    // interrupt handler.
    auto interrupt_handler = [this, callback]() {
      if (callback != nullptr)
      {
        callback();
      }
      // Clear interrupts for all 4 match register interrupt flag
      timer_.peripheral->IR |= 0b1111;
    };

    sjsu::InterruptController::GetPlatformController().Enable({
        .interrupt_request_number = timer_.irq,
        .interrupt_handler        = interrupt_handler,
        .priority                 = priority,
    });

    return Status::kSuccess;
  }

  void SetMatchBehavior(uint32_t ticks,
                        MatchAction action,
                        uint8_t match_register = 0) const override
  {
    SJ2_ASSERT_FATAL(match_register < kMatchRegisterCount,
                     "LPC40xx can only has 3 match registers. An attempt "
                     "to set match register %d was attempted.",
                     match_register);

    timer_.peripheral->MCR =
        bit::Insert(timer_.peripheral->MCR,
                    static_cast<uint32_t>(action),
                    {
                        .position = static_cast<uint8_t>(match_register * 3),
                        .width    = 3,
                    });

    // MR0, MR1, MR2, and MR3 are contiguous, so we can point to the first
    // match register and index from there to get the other match registers.
    volatile uint32_t * match_register_ptr = &timer_.peripheral->MR0;

    match_register_ptr[match_register & 0b11] = ticks / 2;
  }

  uint8_t GetAvailableMatchRegisters() const override
  {
    return kMatchRegisterCount;
  }

  uint32_t GetCount() const override
  {
    return timer_.peripheral->TC;
  }

 private:
  const Peripheral_t & timer_;
  const sjsu::SystemController & system_controller_;
};
}  // namespace lpc40xx
}  // namespace sjsu

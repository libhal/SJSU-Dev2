#pragma once

#include <cstdint>
#include <limits>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/timer.hpp"
#include "utility/bit.hpp"
#include "utility/log.hpp"
#include "utility/error_handling.hpp"
#include "utility/units.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// Implementation of the Timer peripheral for the LPC40xx family of
/// microcontrollers.
class Timer final : public sjsu::Timer
{
 public:
  /// LPC40xx Peripheral descriptor object with all of the information to manage
  /// the Timer peripheral.
  struct Peripheral_t
  {
    /// Address to the timer peripheral registers
    LPC_TIM_TypeDef * peripheral;
    /// ID of the timer peripheral (used to power on the peripheral)
    sjsu::SystemController::ResourceID id;
    /// Interrupt Request Number for this timer
    int irq;
  };

  /// Namespace containing the available peripherals for the LPC40xx platform
  struct Peripheral  // NOLINT
  {
    /// Peripheral descriptor for TIMER 0
    inline static const Peripheral_t kTimer0 = {
      .peripheral = LPC_TIM0,
      .id         = sjsu::lpc40xx::SystemController::Peripherals::kTimer0,
      .irq        = IRQn::TIMER0_IRQn,
    };

    /// Peripheral descriptor for TIMER 1
    inline static const Peripheral_t kTimer1 = {
      .peripheral = LPC_TIM1,
      .id         = sjsu::lpc40xx::SystemController::Peripherals::kTimer1,
      .irq        = IRQn::TIMER1_IRQn,
    };

    /// Peripheral descriptor for TIMER 2
    inline static const Peripheral_t kTimer2 = {
      .peripheral = LPC_TIM2,
      .id         = sjsu::lpc40xx::SystemController::Peripherals::kTimer2,
      .irq        = IRQn::TIMER2_IRQn,
    };

    /// Peripheral descriptor for TIMER 3
    inline static const Peripheral_t kTimer3 = {
      .peripheral = LPC_TIM3,
      .id         = sjsu::lpc40xx::SystemController::Peripherals::kTimer3,
      .irq        = IRQn::TIMER3_IRQn,
    };
  };

  /// Contains common registers for timer control register
  struct TimerControlRegister  // NOLINT
  {
   public:
    /// Enable bit
    inline static constexpr bit::Mask kEnableBit = bit::MaskFromRange(0);

    /// Reset bit
    inline static constexpr bit::Mask kResetBit = bit::MaskFromRange(1);
  };

  /// Constructor of the LPC40xx Timer implementation
  ///
  /// @param timer - a timer peripheral descriptor that, which is the the timer
  /// peripheral to be used with this object
  explicit constexpr Timer(const Peripheral_t & timer) : timer_(timer) {}

  void Initialize(units::frequency::hertz_t frequency,
                  InterruptCallback callback = nullptr,
                  int32_t priority           = -1) const override
  {
    auto & system = sjsu::SystemController::GetPlatformController();
    system.PowerUpPeripheral(timer_.id);

    SJ2_ASSERT_FATAL(
        frequency.to<uint32_t>() != 0,
        "Cannot have zero ticks per microsecond, please choose 1 or more.");

    // Set Prescale register for Prescale Counter to milliseconds
    auto peripheral_frequency = system.GetClockRate(timer_.id);
    uint32_t prescaler        = peripheral_frequency / frequency;
    timer_.peripheral->PR     = prescaler;

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
  }

  void SetMatchBehavior(uint32_t ticks,
                        MatchAction action,
                        uint8_t match_register = 0) const override
  {
    if (match_register >= GetAvailableMatchRegisters())
    {
      LogDebug("match_register = %u", match_register);
      throw Exception(std::errc::invalid_argument,
                      "LPC40xx can only has 3 match registers.");
    }

    timer_.peripheral->MCR =
        bit::Insert(timer_.peripheral->MCR, Value(action),
                    {
                        .position = static_cast<uint8_t>(match_register * 3),
                        .width    = 3,
                    });

    // MR0, MR1, MR2, and MR3 are contiguous, so we can point to the first
    // match register and index from there to get the other match registers.
    volatile uint32_t * match_register_ptr = &timer_.peripheral->MR0;

    match_register_ptr[match_register & 0b11] = ticks;
  }

  uint8_t GetAvailableMatchRegisters() const override
  {
    return 4;
  }

  uint32_t GetCount() const override
  {
    return timer_.peripheral->TC;
  }

  void Start() const override
  {
    timer_.peripheral->TCR = bit::Set(
        timer_.peripheral->TCR, TimerControlRegister::kEnableBit.position);
  }

  void Stop() const override
  {
    timer_.peripheral->TCR = bit::Clear(
        timer_.peripheral->TCR, TimerControlRegister::kEnableBit.position);
  }

  void Reset() const override
  {
    timer_.peripheral->TCR = bit::Set(timer_.peripheral->TCR,
                                      TimerControlRegister::kResetBit.position);
    timer_.peripheral->TCR = bit::Clear(
        timer_.peripheral->TCR, TimerControlRegister::kResetBit.position);
  }

 private:
  const Peripheral_t & timer_;
};
}  // namespace lpc40xx
}  // namespace sjsu

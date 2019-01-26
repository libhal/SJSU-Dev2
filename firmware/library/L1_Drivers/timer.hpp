#pragma once
#include <cstdint>
#include <limits>

#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/system_controller.hpp"
#include "L1_Drivers/pin.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"

class TimerInterface
{
 public:
  enum RegisterChoice : uint8_t
  {
    kRegMR0 = 0,
    kRegMR1 = 1,
    kRegMR2 = 2,
    kRegMR3 = 3,
    kRegCR0 = 4,
    kRegCR1 = 5
  };
  enum TimerPort : uint8_t
  {
    kTimer0 = 0,
    kTimer1,
    kTimer2,
    kTimer3,
    kCount
  };
  enum MatchControlRegister : uint8_t
  {
    kMat0 = 0,
    kMat1 = 3,
    kMat2 = 6,
    kMat3 = 9
  };
  // Modes for each match register that when match register matches timer
  // system can trigger an Interrupt, Stop Counter, or Restart and in any
  // mixture of those commands.
  enum TimerIsrCondition : uint8_t
  {
    kInterrupt            = 0b001,
    kRestart              = 0b010,
    kStop                 = 0b100,
    kInterruptRestart     = 0b011,
    kInterruptStop        = 0b101,
    kRestartStop          = 0b110,
    kInterruptRestartStop = 0b111
  };
  virtual void Initialize(uint32_t us_per_tick, IsrPointer isr,
                          int32_t priority)               = 0;
  virtual void SetTimer(uint32_t time, TimerIsrCondition mode,
                        MatchControlRegister reg = kMat0) = 0;
  virtual uint32_t GetTimer()                             = 0;
};

class Timer : public TimerInterface, protected Lpc40xxSystemController
{
 public:
  inline static LPC_TIM_TypeDef * tim_register[4] = {
    [0] = LPC_TIM0, [1] = LPC_TIM1, [2] = LPC_TIM2, [3] = LPC_TIM3
  };
  static constexpr uint32_t kClear = std::numeric_limits<uint32_t>::max();
  static constexpr Lpc40xxSystemController::PeripheralID kPowerbit[] = {
    Lpc40xxSystemController::Peripherals::kTimer0,
    Lpc40xxSystemController::Peripherals::kTimer1,
    Lpc40xxSystemController::Peripherals::kTimer2,
    Lpc40xxSystemController::Peripherals::kTimer3
  };

  static constexpr IRQn kTimerIrq[] = { IRQn::TIMER0_IRQn, IRQn::TIMER1_IRQn,
                                        IRQn::TIMER2_IRQn, IRQn::TIMER3_IRQn };
  static void DoNothingIsr() {}

  inline static volatile uint32_t * match[4][4] = {
    [kTimer0] = { &tim_register[0]->MR0, &tim_register[0]->MR1,
                  &tim_register[0]->MR2, &tim_register[0]->MR3 },
    [kTimer1] = { &tim_register[1]->MR0, &tim_register[1]->MR1,
                  &tim_register[1]->MR2, &tim_register[1]->MR3 },
    [kTimer2] = { &tim_register[2]->MR0, &tim_register[2]->MR1,
                  &tim_register[2]->MR2, &tim_register[2]->MR3 },
    [kTimer3] = { &tim_register[3]->MR0, &tim_register[3]->MR1,
                  &tim_register[3]->MR2, &tim_register[3]->MR3 }
  };

  inline static IsrPointer user_timer_isr[util::Value(kCount)] = { nullptr };

  template <TimerPort port>
  static void ClearInterrupts()
  {
    uint8_t channel = util::Value(port);
    tim_register[channel]->IR |= (1 << kRegMR0);
    tim_register[channel]->IR |= (1 << kRegMR1);
    tim_register[channel]->IR |= (1 << kRegMR2);
    tim_register[channel]->IR |= (1 << kRegMR3);
  }

  template <TimerPort port>
  static void TimerHandler()
  {
    if (user_timer_isr != nullptr)
    {
      user_timer_isr[util::Value(port)]();
    }
    ClearInterrupts<port>();
  }

  static constexpr IsrPointer kTimerIsr[] = {
    TimerHandler<TimerPort::kTimer0>,
    TimerHandler<TimerPort::kTimer1>,
    TimerHandler<TimerPort::kTimer2>,
    TimerHandler<TimerPort::kTimer3>,
  };

  explicit constexpr Timer(TimerPort mode) : channel_(util::Value(mode)) {}

  /// @param frequency the frequency that the timer register (TC) will
  ///                  increment by. If set to 1000Hz, after 10 ms the TC
  ///                  register will be 10 ms.
  /// @param isr an ISR that will fire when the condition set by SetTimer
  ///            method is achieved.
  /// @param priority sets the Timer interrupt's priority level, defaults to -1
  ///                 which uses the platforms default priority.
  void Initialize(uint32_t frequency, IsrPointer isr = DoNothingIsr,
                  int32_t priority = -1) override final
  {
    PowerUpPeripheral(kPowerbit[channel_]);
    SJ2_ASSERT_FATAL(
        frequency != 0,
        "Cannot have zero ticks per microsecond, please choose 1 or more.");
    // Set Prescale register for Prescale Counter to milliseconds
    uint32_t prescaler = GetPeripheralFrequency() / frequency;
    tim_register[channel_]->PR &= ~(kClear << 1);
    tim_register[channel_]->PR |= (prescaler << 1);
    tim_register[channel_]->TCR |= (1 << 0);
    user_timer_isr[channel_] = isr;
    RegisterIsr(kTimerIrq[channel_], kTimerIsr[channel_], true, priority);
  }

  /// @description Function that sets a timer of your choice (0-3) to a specific
  /// time in milliseconds or micro seconds dependent on initialization
  /// Functionality is defined by mode: interrupt, stop, or reset on match.
  ///
  /// @param ticks the count of the timer register (TC) to have an ISR fire
  /// @param condition the condition for which a timer interrupt will occur
  /// @param match_register which match register (from 0 to 3) should be used
  ///                       for holding the ticks for the condition.
  void SetTimer(uint32_t ticks, TimerIsrCondition condition,
                MatchControlRegister match_register = kMat0) override final
  {
    static constexpr uint8_t kClearMode = 0b0111;

    uint8_t match_value = util::Value(match_register);
    tim_register[channel_]->MCR &= ~(kClearMode << match_value);
    tim_register[channel_]->MCR |= condition << match_value;
    *match[channel_][match_value / 3] |= (((ticks / 2)) << 0);
  }

  [[gnu::always_inline]] uint32_t GetTimer() override final {
    return tim_register[channel_]->TC;
  }

 private:
  uint8_t channel_;
};

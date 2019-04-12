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

  virtual void Initialize(uint32_t us_per_tick, IsrPointer isr = nullptr,
                          int32_t priority = -1) const = 0;
  virtual void SetTimer(uint32_t time, TimerIsrCondition mode,
                        uint8_t reg = 0) const         = 0;
  virtual uint32_t GetTimer() const                    = 0;
};

class Timer final : public TimerInterface, protected Lpc40xxSystemController
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

  struct ChannelPartial_t
  {
    LPC_TIM_TypeDef * timer_register;
    PeripheralID power_id;
    IRQn irq;
    IsrPointer * user_callback;
  };

  template <const ChannelPartial_t & port>
  static void TimerHandler()
  {
    TimerHandler(port);
  }

  struct Channel_t
  {
    const ChannelPartial_t & channel;
    IsrPointer isr;
  };

  struct Channel  // NOLINT
  {
   private:
    inline static IsrPointer timer0_isr                 = nullptr;
    inline static const ChannelPartial_t kTimerPartial0 = {
      .timer_register = LPC_TIM0,
      .power_id       = Peripherals::kTimer0,
      .irq            = IRQn::TIMER0_IRQn,
      .user_callback  = &timer0_isr,
    };

    inline static IsrPointer timer1_isr                 = nullptr;
    inline static const ChannelPartial_t kTimerPartial1 = {
      .timer_register = LPC_TIM1,
      .power_id       = Peripherals::kTimer1,
      .irq            = IRQn::TIMER1_IRQn,
      .user_callback  = &timer1_isr,
    };

    inline static IsrPointer timer2_isr                 = nullptr;
    inline static const ChannelPartial_t kTimerPartial2 = {
      .timer_register = LPC_TIM2,
      .power_id       = Peripherals::kTimer2,
      .irq            = IRQn::TIMER2_IRQn,
      .user_callback  = &timer2_isr,
    };

    inline static IsrPointer timer3_isr                 = nullptr;
    inline static const ChannelPartial_t kTimerPartial3 = {
      .timer_register = LPC_TIM3,
      .power_id       = Peripherals::kTimer3,
      .irq            = IRQn::TIMER3_IRQn,
      .user_callback  = &timer3_isr,
    };

   public:
    inline static const Channel_t kTimer0 = {
      .channel = kTimerPartial0,
      .isr     = TimerHandler<kTimerPartial0>,
    };

    inline static const Channel_t kTimer1 = {
      .channel = kTimerPartial1,
      .isr     = TimerHandler<kTimerPartial1>,
    };

    inline static const Channel_t kTimer2 = {
      .channel = kTimerPartial2,
      .isr     = TimerHandler<kTimerPartial2>,
    };

    inline static const Channel_t kTimer3 = {
      .channel = kTimerPartial3,
      .isr     = TimerHandler<kTimerPartial3>,
    };
  };

  static void TimerHandler(const ChannelPartial_t & channel)
  {
    if (*channel.user_callback != nullptr)
    {
      (*channel.user_callback)();
    }
    channel.timer_register->IR |=
        (1 << kRegMR0) | (1 << kRegMR1) | (1 << kRegMR2) | (1 << kRegMR3);
  }

  explicit constexpr Timer(Channel_t timer) : timer_(timer) {}

  /// @param frequency - the frequency that the timer register (TC) will
  ///        increment by. If set to 1000Hz, after 10 ms the TC
  ///        register will be 10 ms.
  /// @param isr - an ISR that will fire when the condition set by SetTimer
  ///        method is achieved.
  /// @param priority - sets the Timer interrupt's priority level, defaults to
  ///        -1 which uses the platforms default priority.
  void Initialize(uint32_t frequency, IsrPointer isr = nullptr,
                  int32_t priority = -1) const override
  {
    constexpr uint32_t kClear = std::numeric_limits<uint32_t>::max();
    PowerUpPeripheral(timer_.channel.power_id);
    SJ2_ASSERT_FATAL(
        frequency != 0,
        "Cannot have zero ticks per microsecond, please choose 1 or more.");
    // Set Prescale register for Prescale Counter to milliseconds
    uint32_t prescaler = GetPeripheralFrequency() / frequency;
    timer_.channel.timer_register->PR &= ~(kClear << 1);
    timer_.channel.timer_register->PR |= (prescaler << 1);
    timer_.channel.timer_register->TCR |= (1 << 0);
    *timer_.channel.user_callback = isr;
    RegisterIsr(timer_.channel.irq, timer_.isr, true, priority);
  }

  /// Function that sets a timer of your choice (0-3) to a specific
  /// time in milliseconds or micro seconds dependent on initialization
  /// Functionality is defined by mode: interrupt, stop, or reset on match.
  ///
  /// @param ticks - the count of the timer register (TC) to have an ISR fire
  /// @param condition - the condition for which a timer interrupt will occur
  /// @param match_register - which match register (from 0 to 3) should be used
  ///        for holding the ticks for the condition. Only the two least
  ///        significant bits are used for the LPC40xx.
  void SetTimer(uint32_t ticks, TimerIsrCondition condition,
                uint8_t match_register = 0) const override
  {
    SJ2_ASSERT_FATAL(match_register > 3,
                     "The LPC40xx can only has 3 match registers. An attempt "
                     "to set match register %d was attempted.",
                     match_register);

    static constexpr uint8_t kClearMode = 0b0111;

    timer_.channel.timer_register->MCR &= ~(kClearMode << match_register);
    timer_.channel.timer_register->MCR |= condition << match_register;
    // MR0, MR1, MR2, and MR3 are contiguous, so we can point to the first
    // match register and index from there to get the other match registers.
    volatile uint32_t * match_register_ptr =
        &timer_.channel.timer_register->MR0;

    match_register_ptr[match_register & 0b11] = ticks / 2;
  }

  uint32_t GetTimer() const override
  {
    return timer_.channel.timer_register->TC;
  }

 private:
  Channel_t & timer_;
};

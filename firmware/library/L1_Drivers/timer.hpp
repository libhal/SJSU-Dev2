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
    kTimer1 = 1,
    kTimer2 = 2,
    kTimer3 = 3
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
  enum TimerSetting : uint8_t
  {
    kInterrupt            = 0b001,
    kRestart              = 0b010,
    kStop                 = 0b100,
    kInterruptRestart     = 0b011,
    kInterruptStop        = 0b101,
    kRestartStop          = 0b110,
    kInterruptRestartStop = 0b111
  };
  virtual void Initialize(TimerPort timer, uint32_t us_per_tick, IsrPointer isr,
                          int32_t priority) = 0;
  virtual void SetTimer(uint32_t time, MatchControlRegister reg,
                        TimerSetting mode)  = 0;
  virtual uint32_t GetTimer()               = 0;
  virtual void ClearInterrupts()            = 0;
};

class Timer : public TimerInterface, protected Lpc40xxSystemController
{
 public:
  inline static LPC_TIM_TypeDef * tim_register[4] = {
    [0] = LPC_TIM0, [1] = LPC_TIM1, [2] = LPC_TIM2, [3] = LPC_TIM3
  };
  static constexpr uint8_t kTimerFunc = 0b011;
  static constexpr uint8_t kClearMode = 0b0111;
  static constexpr uint32_t kClear    = std::numeric_limits<uint32_t>::max();
  static constexpr Lpc40xxSystemController::PeripheralID kPowerbit[] = {
    Lpc40xxSystemController::Peripherals::kTimer0,
    Lpc40xxSystemController::Peripherals::kTimer1,
    Lpc40xxSystemController::Peripherals::kTimer2,
    Lpc40xxSystemController::Peripherals::kTimer3
  };

  static constexpr IRQn kRegISR[] = { IRQn::TIMER0_IRQn, IRQn::TIMER1_IRQn,
                                      IRQn::TIMER2_IRQn, IRQn::TIMER3_IRQn };

  explicit constexpr Timer(TimerPort mode) : channel_(util::Value(mode)) {}

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

  void Initialize(TimerPort timer, uint32_t us_per_tick, IsrPointer isr,
                  int32_t priority) override final
  {
    PowerUpPeripheral(kPowerbit[channel_]);
    SJ2_ASSERT_FATAL(
        us_per_tick > 0,
        "Cannot have zero ticks per microsecond, please choose 1 or more.");
    // Set Prescale register for Prescale Counter to milliseconds
    uint32_t freq       = ((GetPeripheralFrequency() / 1000000) * us_per_tick);
    uint8_t timer_index = util::Value(timer);
    tim_register[timer_index]->PR &= ~(kClear << 1);
    tim_register[timer_index]->PR |= (freq << 1);
    tim_register[timer_index]->TCR |= (1 << 0);
    RegisterIsr(kRegISR[channel_], isr, true, priority);
  }

  // Function that sets a timer of your choice (0-3) to a specific
  // time in milliseconds or micro seconds dependent on initilization
  // Functionality is defined by mode: interrupt, stop, or reset on match.
  void SetTimer(uint32_t time, MatchControlRegister reg,
                TimerSetting mode) override final
  {
    tim_register[channel_]->MCR &= ~(kClearMode << util::Value(reg));
    tim_register[channel_]->MCR |= mode << util::Value(reg);
    *match[channel_][(util::Value(reg)) / 3] |= (((time / 2)) << 0);
  }

  [[gnu::always_inline]] uint32_t GetTimer() override final
  {
    return tim_register[channel_]->TC;
  }

  void ClearInterrupts() override final
  {
    tim_register[channel_]->IR |= (1 << kRegMR0);
    tim_register[channel_]->IR |= (1 << kRegMR1);
    tim_register[channel_]->IR |= (1 << kRegMR2);
    tim_register[channel_]->IR |= (1 << kRegMR3);
  }

 private:
  uint8_t channel_;
};

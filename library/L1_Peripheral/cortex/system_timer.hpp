// SystemTimer abstracts the process of changing enabling and setting
// up the SystemTimer.
#pragma once

#include "L0_Platform/interrupt.hpp"
// NOTE: Including lpc40xx's definitions includes SysTick address and
// definitions as well. These definitions are the same across m3 and m4, so its
// fine to include this here.
#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/system_controller.hpp"
#include "L1_Peripheral/system_timer.hpp"
#include "utility/status.hpp"

using ::sjsu::lpc40xx::SysTick_Type;

namespace sjsu
{
namespace cortex
{
class SystemTimer final : public sjsu::SystemTimer
{
 public:
  // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
  enum ControlBitMap : uint8_t
  {
    kEnableCounter = 0,
    kTickInterupt  = 1,
    kClkSource     = 2,
    kCountFlag     = 16
  };

  inline static SysTick_Type * sys_tick = SysTick;
  /// system_timer_isr defaults to nullptr. The actual SystemTickHandler should
  /// check if the isr is set to nullptr, and if it is, turn off the timer, if
  /// set a proper function then execute it.
  inline static IsrPointer system_timer_isr = nullptr;
  /// Used to count the number of times system_timer has executed. If the
  /// frequency of the SystemTimer is set to 1kHz, this could be used as a
  /// milliseconds counter.
  inline static uint64_t counter = 0;
  // If the user does not specify a system controller, the default system
  // controller will be used. The default system controller does not modify
  // hardware. It only returns the project's desired system clock rate.
  inline static DefaultSystemController default_system_controller;

  explicit SystemTimer(
      const SystemController & system_controller = default_system_controller)
      : system_controller_(system_controller)
  {
  }

  /// WARNING: Doing so will most likely disable FreeRTOS
  static void DisableTimer()
  {
    sys_tick->LOAD = 0;
    sys_tick->VAL  = 0;
    sys_tick->CTRL = 0;
  }
  static void SystemTimerHandler()
  {
    counter++;
    // This assumes that SysTickHandler is called every millisecond.
    // Changing that frequency will distort the milliseconds time.
    if (system_timer_isr != nullptr)
    {
      system_timer_isr();
    }
  }
  static uint64_t GetCount()
  {
    return counter;
  }

  void SetInterrupt(IsrPointer isr) const override
  {
    system_timer_isr = isr;
  }

  Status StartTimer() const override
  {
    Status status = Status::kInvalidSettings;

    if (sys_tick->LOAD != 0)
    {
      sys_tick->VAL = 0;
      sys_tick->CTRL |= (1 << ControlBitMap::kTickInterupt);
      sys_tick->CTRL |= (1 << ControlBitMap::kEnableCounter);
      sys_tick->CTRL |= (1 << ControlBitMap::kClkSource);

      RegisterIsr(lpc40xx::SysTick_IRQn, SystemTimerHandler);
      status = Status::kSuccess;
    }

    return status;
  }
  /// @param frequency set the frequency that SystemTick counter will run.
  ///        If it is above the maximum SystemTick value 2^24
  ///        [SysTick_LOAD_RELOAD_Msk], the value is ceiled to
  ///        SysTick_LOAD_RELOAD_Msk.
  /// @returns if the freqency was not divisible by the clock frequency, the
  ///          remainder will be returned.
  ///          If the freqency supplied is less then 2Hz, the function will
  ///          return without changing any hardware.
  ///          If the reload value exceeds the SysTick_LOAD_RELOAD_Msk, the
  ///          returned value is the SysTick_LOAD_RELOAD_Msk.
  uint32_t SetTickFrequency(uint32_t frequency) const override
  {
    if (frequency <= 1)
    {
      return 0;
    }

    uint32_t system_frequency = system_controller_.GetSystemFrequency();
    uint32_t reload_value     = (system_frequency / frequency) - 1;
    int remainder             = (system_frequency % frequency);

    if (reload_value > SysTick_LOAD_RELOAD_Msk)
    {
      reload_value = SysTick_LOAD_RELOAD_Msk;
      remainder    = SysTick_LOAD_RELOAD_Msk;
    }

    sys_tick->LOAD = reload_value;
    return remainder;
  }

 private:
  const SystemController & system_controller_;
};
}  // namespace cortex
}  // namespace sjsu

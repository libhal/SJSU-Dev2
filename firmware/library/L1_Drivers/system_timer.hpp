// SystemTimer abstracts the process of changing enabling and setting
// up the SystemTimer.
#pragma once

#include <cstring>

#include "config.hpp"

#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/system_controller.hpp"
#include "utility/macros.hpp"

class SystemTimerInterface
{
 public:
  virtual void SetIsrFunction(IsrPointer isr)           = 0;
  virtual bool StartTimer()                             = 0;
  virtual uint32_t SetTickFrequency(uint32_t frequency) = 0;
};

class SystemTimer final : public SystemTimerInterface,
                          protected Lpc40xxSystemController
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
  /// Sys_tick structure defaults to the Core M4 SysTick register address found
  /// in L0_LowLevel/SystemFiles/core_m4.h which is included in LPC40xx.h
  inline static SysTick_Type * sys_tick = SysTick;
  /// system_timer_isr defaults to nullptr. The actual SystemTickHandler should
  /// check if the isr is set to nullptr, and if it is, turn off the timer, if
  /// set a proper function then execute it.
  inline static IsrPointer system_timer_isr = nullptr;
  /// WARNING: Doing so will most likely disable FreeRTOS
  static void DisableTimer()
  {
    sys_tick->LOAD = 0;
    sys_tick->VAL  = 0;
    sys_tick->CTRL = 0;
  }
  static void SystemTimerHandler()
  {
    // This assumes that SysTickHandler is called every millisecond.
    // Changing that frequency will distort the milliseconds time.
    if (system_timer_isr != nullptr)
    {
      system_timer_isr();
    }
  }
  constexpr SystemTimer() {}
  void SetIsrFunction(IsrPointer isr) override
  {
    system_timer_isr = isr;
  }
  bool StartTimer() override
  {
    bool successful = false;
    if (sys_tick->LOAD != 0)
    {
      sys_tick->VAL = 0;
      sys_tick->CTRL |= (1 << ControlBitMap::kTickInterupt);
      sys_tick->CTRL |= (1 << ControlBitMap::kEnableCounter);
      sys_tick->CTRL |= (1 << ControlBitMap::kClkSource);
      successful = true;
    }
    RegisterIsr(SysTick_IRQn, SystemTimerHandler);
    return successful;
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
  uint32_t SetTickFrequency(uint32_t frequency) override
  {
    if (frequency <= 1)
    {
      return 0;
    }
    uint32_t reload_value = (GetSystemFrequency() / frequency) - 1;
    int remainder         = (GetSystemFrequency() % frequency);
    if (reload_value > SysTick_LOAD_RELOAD_Msk)
    {
      reload_value = SysTick_LOAD_RELOAD_Msk;
      remainder    = SysTick_LOAD_RELOAD_Msk;
    }
    sys_tick->LOAD = reload_value;
    return remainder;
  }
};

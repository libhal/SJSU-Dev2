// SystemTimer abstracts the process of changing enabling and setting
// up the SystemTimer.
//
//   Usage:
//      Pin P0_0(0, 0);
//      P0_0.SetAsActiveLow();
//      P0_0.SetMode(PinInterface::Mode::kPullUp);
#pragma once

#include <cstring>

#include "config.hpp"
#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L2_Utilities/macros.hpp"

// LPC4076 does not include P3.26 so supporting methods are not available
class SystemTimerInterface
{
 public:
  virtual void SetIsrFunction(IsrPointer isr)           = 0;
  virtual bool StartTimer()                             = 0;
  virtual void DisableTimer()                           = 0;
  virtual uint32_t SetTickFrequency(uint32_t frequency) = 0;
};

class SystemTimer : public SystemTimerInterface
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
  static IsrPointer system_timer_isr;
  static SysTick_Type * sys_tick;

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
    return successful;
  }
  // WARNING: doing so will most likely disable FreeRTOS
  void DisableTimer() override
  {
    sys_tick->LOAD = 0;
    sys_tick->VAL  = 0;
    sys_tick->CTRL = 0;
  }
  // @param frequency set the frequency that SystemTick counter will run.
  //        If it is above the maximum SystemTick value 2^24
  //        [SysTick_LOAD_RELOAD_Msk], the value is ceiled to
  //        SysTick_LOAD_RELOAD_Msk
  uint32_t SetTickFrequency(uint32_t frequency) override
  {
    frequency             = (frequency == 0) ? 1 : frequency;
    uint32_t reload_value = config::kSystemClockRate / frequency - 1;
    int remainder         = config::kSystemClockRate % frequency;
    if (reload_value > SysTick_LOAD_RELOAD_Msk)
    {
      reload_value = SysTick_LOAD_RELOAD_Msk;
      remainder    = SysTick_LOAD_RELOAD_Msk;
    }
    sys_tick->LOAD = reload_value;
    return remainder;
  }
};

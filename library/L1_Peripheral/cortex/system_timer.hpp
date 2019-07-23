// SystemTimer abstracts the process of changing enabling and setting
// up the SystemTimer.
#pragma once

// NOTE: Support for cortex M4 also supports M3 and possibly M0 and M0+ as well.
#include "L0_Platform/arm_cortex/m4/core_cm4.h"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/system_controller.hpp"
#include "L1_Peripheral/system_timer.hpp"
#include "utility/status.hpp"
#include "utility/enum.hpp"

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
  inline static const sjsu::cortex::InterruptController
      kCortexInterruptController = sjsu::cortex::InterruptController();

  explicit SystemTimer(const sjsu::SystemController & system_controller,
                       const sjsu::InterruptController & interrupt_controller =
                           kCortexInterruptController)
      : system_controller_(system_controller),
        interrupt_controller_(interrupt_controller)
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

      interrupt_controller_.Register({
          .interrupt_request_number  = cortex::SysTick_IRQn,
          .interrupt_service_routine = SystemTimerHandler,
      });
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
  const sjsu::SystemController & system_controller_;
  const sjsu::InterruptController & interrupt_controller_;
};
}  // namespace cortex
}  // namespace sjsu

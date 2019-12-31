// SystemTimer abstracts the process of changing enabling and setting
// up the SystemTimer.
#pragma once

#include <functional>

#include "L0_Platform/arm_cortex/m4/core_cm4.h"
#include "L1_Peripheral/cortex/interrupt.hpp"
#include "L1_Peripheral/cortex/dwt_counter.hpp"
#include "L1_Peripheral/system_controller.hpp"
#include "L1_Peripheral/system_timer.hpp"
#include "utility/status.hpp"
#include "utility/enum.hpp"
#include "utility/time.hpp"
#include "utility/units.hpp"

namespace sjsu
{
namespace cortex
{
/// Implementation of the sjsu::SystemTimer for all ARM Cortex-M series
/// microcontrollers.
class SystemTimer final : public sjsu::SystemTimer
{
 public:
  /// Enumeration holding the bit positions of used flags.
  /// Source: "UM10562 LPC408x/407x User manual" table 83 page 132
  enum ControlBitMap : uint8_t
  {
    kEnableCounter = 0,
    kTickInterupt  = 1,
    kClkSource     = 2,
    kCountFlag     = 16
  };

  /// Address of the ARM Cortex SysTick peripheral.
  inline static SysTick_Type * sys_tick = SysTick;

  /// callback defaults to nullptr. The actual SystemTickHandler
  /// should check if the isr is set to nullptr, and if it is, turn off the
  /// timer, if set a proper function then execute it.
  inline static InterruptCallback callback = nullptr;

  /// Used to count the number of times system_timer has executed. If the
  /// frequency of the SystemTimer is set to 1kHz, this could be used as a
  /// milliseconds counter.
  inline static std::chrono::nanoseconds millisecond_count = 0us;

  /// Higher precision counter that counts on every system clock cycle
  inline static DwtCounter dwt_counter;

  /// Holds the converstion from ticks to nanoseconds
  inline static std::chrono::nanoseconds nanoseconds_per_tick = 1us;

  /// Holds the conversion from ticks to milliseconds
  inline static uint32_t ticks_per_millisecond = 1;

  /// Fixed point scale in order to boost the precision of GetCount() method
  /// while also keeping most of the arithmetic as integer math.
  static constexpr uint32_t kFixedPointScaling = 1'000'000;

  /// Disables this system timer.
  /// @warning: Calling this function so will disable FreeRTOS.
  static void DisableTimer()
  {
    sys_tick->LOAD = 0;
    sys_tick->VAL  = 0;
    sys_tick->CTRL = 0;
  }

  /// System timer interrupt handler.
  static void SystemTimerHandler()
  {
    // This assumes that SysTickHandler is called every millisecond.
    // Changing that frequency will distort the milliseconds time.
    millisecond_count += 1ms;
    if (callback)
    {
      callback();
    }
  }

  /// @return returns the current system_timer counter value.
  static std::chrono::nanoseconds GetCount()
  {
    // Capture all count variables at this instant.
    uint32_t high_speed_ticks = dwt_counter.GetCount();
    auto current_millis_count = millisecond_count;

    // Get rid of the portion of the dwt tick count that is above 1ms.
    // We only want the information about the ticks that is below 1ms, since we
    // get the 1ms count from the SysTick timer interrupt.
    uint32_t sub_millis_ticks = (high_speed_ticks % ticks_per_millisecond);
    // Perform the conversion from ticks to nanoseconds
    auto nanoseconds_unscaled = (sub_millis_ticks * nanoseconds_per_tick);
    // Nanoseconds per tick is actually scaled up by the kFixedPointScaling
    // factor, meaning it needs to be divided by that value to get the correct
    // nanosecond_uptime.
    auto nanoseconds_uptime_scaled = nanoseconds_unscaled / kFixedPointScaling;

    // Result is simply the uptime milliseconds (represented in nanoseconds) +
    // the added nanoseconds
    return current_millis_count + nanoseconds_uptime_scaled;
  }

  /// Constructor for ARM Cortex M system timer.
  ///
  /// @param priority - the interrupt priority of
  explicit constexpr SystemTimer(uint8_t priority = -1) : priority_(priority) {}

  void Initialize() const override
  {
    dwt_counter.Initialize();

    auto system_frequency = SystemController::GetPlatformController()
                                .GetSystemFrequency()
                                .to<uint32_t>();

    ticks_per_millisecond = system_frequency / 1000 /* ms/s */;
    nanoseconds_per_tick =
        (kFixedPointScaling * 1'000'000'000ns) / system_frequency;
  }

  void SetCallback(InterruptCallback isr) const override
  {
    callback = isr;
  }

  Status StartTimer() const override
  {
    Status status = Status::kInvalidSettings;

    if (sys_tick->LOAD != 0)
    {
      // The interrupt handler must be registered before you starting the timer
      // by setting the Enable counter flag in the CTRL register.
      // Otherwise, the handler may not be set by the time the first tick
      // interrupt occurs.
      sjsu::InterruptController::GetPlatformController().Enable({
          .interrupt_request_number = cortex::SysTick_IRQn,
          .interrupt_handler        = SystemTimerHandler,
          .priority                 = priority_,
      });
      // Set all flags required to enable the counter
      uint32_t ctrl_mask = (1 << ControlBitMap::kTickInterupt) |
                           (1 << ControlBitMap::kEnableCounter) |
                           (1 << ControlBitMap::kClkSource);
      // Set the system tick counter to start immediately
      sys_tick->VAL = 0;
      sys_tick->CTRL |= ctrl_mask;

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
  ///          If the freqency supplied is less then 1Hz, the function will
  ///          return without changing any hardware and return -1.
  ///          If the reload value exceeds the SysTick_LOAD_RELOAD_Msk, the
  ///          returned value is the SysTick_LOAD_RELOAD_Msk.
  int32_t SetTickFrequency(units::frequency::hertz_t frequency) const override
  {
    if (frequency <= 1_Hz)
    {
      return -1;
    }

    units::frequency::hertz_t system_frequency =
        sjsu::SystemController::GetPlatformController().GetSystemFrequency();

    uint32_t reload_value =
        units::unit_cast<uint32_t>((system_frequency / frequency) - 1);

    int remainder = (units::unit_cast<uint32_t>(system_frequency) %
                     units::unit_cast<uint32_t>(frequency));

    if (reload_value > SysTick_LOAD_RELOAD_Msk)
    {
      reload_value = SysTick_LOAD_RELOAD_Msk;
      remainder    = SysTick_LOAD_RELOAD_Msk;
    }

    sys_tick->LOAD = reload_value;
    return remainder;
  }

 private:
  uint8_t priority_;
};
}  // namespace cortex
}  // namespace sjsu

// SystemTimer abstracts the process of changing enabling and setting
// up the SystemTimer.
#pragma once

#include <cstdint>

#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/interrupt.hpp"
#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// A system timer is a general timer used primarily for generating an interrupt
/// at a fixed period, like 1ms or 10ms. Such interrupts are generally used to
/// give control of the processor back to an operating.
/// @ingroup l1_peripheral
class SystemTimer : public Module
{
 public:
  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Configuration Methods
  // ---------------------------------------------------------------------------

  /// Set the function to be called when the System Timer interrupt fires.
  ///
  /// @param callback - function to be called on system timer event.
  virtual void ConfigureCallback(InterruptCallback callback) = 0;

  /// Set frequency of the timer.
  ///
  /// @param frequency - How many times per second should the system timer
  ///         interrupt be called.
  virtual void ConfigureTickFrequency(units::frequency::hertz_t frequency) = 0;

  // ---------------------------------------------------------------------------
  // Usage Methods
  // ---------------------------------------------------------------------------
};

/// Template specialization that generates an inactive sjsu::SystemTimer.
template <>
inline sjsu::SystemTimer & GetInactive<sjsu::SystemTimer>()
{
  class InactiveSystemTimer : public sjsu::SystemTimer
  {
   public:
    void ModuleInitialize() override {}
    void ModuleEnable(bool = true) override {}
    void ConfigureCallback(InterruptCallback) override {}
    void ConfigureTickFrequency(units::frequency::hertz_t) override {}
  };

  static InactiveSystemTimer inactive;
  return inactive;
}
}  // namespace sjsu

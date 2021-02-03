// SystemTimer abstracts the process of changing enabling and setting
// up the SystemTimer.
#pragma once

#include <cstdint>

#include "peripherals/inactive.hpp"
#include "peripherals/interrupt.hpp"
#include "config.hpp"
#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/math/units.hpp"

namespace sjsu
{
/// Generic settings for a standard System Timer peripheral
struct SystemTimerSettings_t
{
  /// The operating frequency of the system timer. This specifies how often the
  /// callback is called. Setting this to 1 kHz will result in callback being
  /// called every 1ms.
  units::frequency::hertz_t frequency = config::kRtosFrequency;

  /// The callback to be executed based on the operating frequency.
  InterruptCallback callback = []() {};
};

/// A system timer is a general timer used primarily for generating an interrupt
/// at a fixed period, like 1ms or 10ms. Such interrupts are generally used to
/// give control of the processor back to an operating.
/// @ingroup l1_peripheral
class SystemTimer : public Module<SystemTimerSettings_t>
{
};

/// Template specialization that generates an inactive sjsu::SystemTimer.
template <>
inline sjsu::SystemTimer & GetInactive<sjsu::SystemTimer>()
{
  class InactiveSystemTimer : public sjsu::SystemTimer
  {
   public:
    void ModuleInitialize() override {}
  };

  static InactiveSystemTimer inactive;
  return inactive;
}
}  // namespace sjsu

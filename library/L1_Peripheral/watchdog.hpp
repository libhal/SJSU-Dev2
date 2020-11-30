#pragma once

#include <cstdint>
#include "third_party/units/units.h"
#include "utility/error_handling.hpp"

namespace sjsu
{
/// Abstract interface for a hardware watchdog timer peripheral which can be
/// used to determine if the system has become locked up, and if so, restarts
/// the system.
///
/// (INCOMPLETE PERIPHERAL does not support sjsu::Module)
/// @ingroup l1_peripheral
class Watchdog
{
  // TODO(#998): May need to expand the set of functions watchdogs perform.
  //             At this moment, all watchdog implementations reset the
  //             processor.
 public:
  /// Initialize watchdog peripheral and give it an initial feeding interval.
  ///
  /// @param interval - feeding interval. Not feeding the watch dog in this
  ///        time will result in the system restarting.
  /// @return status indicating the failure type for the watchdog.
  virtual void Initialize(std::chrono::seconds interval) const = 0;

  /// Enables the watchdog. After this point, the watch dog must be feed before
  /// the interval duration is exceeded, otherwise the system will restart.
  virtual void Enable() const = 0;

  /// Feeds the watchdog and restarts the sequence.
  virtual void FeedSequence() const = 0;

  /// Reads the current counter value of the watchdog timer.
  virtual uint32_t CheckTimeout() const = 0;
};
}  // namespace sjsu

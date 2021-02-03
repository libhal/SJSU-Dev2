#pragma once

#include "utility/math/units.hpp"
#include "utility/time/time.hpp"

namespace sjsu
{
/// TimeoutTimer is a utility that helps keep track of the amount of time left
/// before a timeout occurs. This is useful when performing an operation that
/// cannot exceed a certain amount of time, and apart of this operation is
/// calling other functions or operations that take a timeout. The idea is to
///
/// See devices/communication/esp8266.hpp for good examples of TimeoutTimer
/// being used.
class TimeoutTimer
{
 public:
  /// @param timeout - the amount of time before this timer expires. For
  /// example, if you set this to 1s, then this timer will timeout after 1s has
  /// elapsed in real time.
  explicit TimeoutTimer(std::chrono::nanoseconds timeout) : future_timeout_{ 0 }
  {
    SetNewTimeout(timeout);
  }

  /// Returns the amount of time left in the timer. The values will return
  /// negative. If the timer has expired.
  std::chrono::nanoseconds GetTimeLeft()
  {
    return future_timeout_ - Uptime();
  }

  /// Sets a new future deadline for the timer just like during the construction
  /// of this object. Information about the previous timeout start time is
  /// lost.
  ///
  /// @param new_timeout - the new timeout to set this timer to.
  void SetNewTimeout(std::chrono::nanoseconds new_timeout)
  {
    future_timeout_ = Uptime() + new_timeout;
  }

  /// This is useful if you want to perform a loop or particular task where you
  /// can continually check (poll) on this object to determine if the operation
  /// has surpassed the alloted amount of time.
  ///
  /// @return true if there is no more time left in the timeout timer.
  bool HasExpired()
  {
    return GetTimeLeft() < 0ns;
  }

 private:
  std::chrono::nanoseconds future_timeout_;
};
}  // namespace sjsu

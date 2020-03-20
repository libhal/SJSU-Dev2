#pragma once

#include "utility/time.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// TimeoutTimer is a utility that helps keep track of the amount of time left
/// before a timeout occurs. This is useful when performing an operation that
/// cannot exceed a certain amount of time, and apart of this operation is
/// calling other functions or operations that take a timeout. The idea is to
///
/// Example Usage:
///
///   Status_t GetWaterLevel(std::chrono::nanoseconds timeout)
///   {
///     // Construct TimeoutTimer with the given timeout above. Lets assume it
///     // is 1 second.
///     TimeoutTimer timeout(timeout);
///     // An example device that takes time to perform its operations.
///     WaterLevelDevice water_level;
///     // Reusable status variable
///     Status_t status;
///
///     // In this case, we give the remaining time left for calibration
///     // operation. Lets assume that the time is still around 1 second. The
///     // `CalibrateSensor()` method now has 1s to perform its work.
///     status = water_level.CalibrateSensor(timeout.GetTimeLeft());
///     if (!IsOk(status))
///     {
///       return status;
///     }
///
///     // Lets now assume that `CalibrateSensor()` took about 250ms to perform
///     // its task. Now we pass 750ms to `MeasureWaterLevelWhenSteady()`.
///     status = water_level.MeasureWaterLevelWhenSteady(timeout.GetTimeLeft());
///     // If `MeasureWaterLevelWhenSteady()` reached the timeout of 750ms,
///     // then we return with Status::kTimeout.
///     // If we still have time left, and no other problems occurred, then
///     // status will return Status::kSuccess.
///     if (!IsOk(status))
///     {
///       return status;
///     }
///
///     return Status::kSuccess;
///   }
///
/// See L2_HAL/communication/esp8266.hpp for good examples of TimeoutTimer being
/// used.
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

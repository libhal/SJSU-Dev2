// Usage:
//
//    sjsu::StopWatch stopwatch;
//    stopwatch.Calibrate();
//    stopwatch.Start();
//
//    // Do some work that takes some time to perform ...
//
//    auto time_delta = stopwatch.Stop();
//
#pragma once

#include "utility/time.hpp"

namespace sjsu
{
/// A stop watch class that can be used to determine the time between events.
class StopWatch
{
 public:
  /// Calibrates the stopwatch by figuring out how much time it takes to Start()
  /// and Stop() the stopwatch. Since it takes time to call the methods, this is
  /// used to remove the call to Stop() and Start() from the time between
  /// events, giving a more accurate time.
  void Calibrate()
  {
    calibrate_delta_ = 0us;
    Start();
    calibrate_delta_ = Stop();
  }

  /// Acquires the current uptime and stores it for comparision against the time
  /// in which Stop() is called.
  void Start()
  {
    start_ticks_ = Uptime();
  }

  /// Calculates and returns the current lap/time delta from the previous time
  /// Start() was called.
  std::chrono::microseconds Stop()
  {
    auto current_uptime = Uptime();
    return (current_uptime - start_ticks_) - calibrate_delta_;
  }

  /// This is used to inspect the calibration delta time.
  std::chrono::microseconds GetCalibrationDelta()
  {
    return calibrate_delta_;
  }

 private:
  std::chrono::microseconds calibrate_delta_ = 0us;
  std::chrono::microseconds start_ticks_     = 0us;
};
}  // namespace sjsu

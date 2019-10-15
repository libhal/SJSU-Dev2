// stopwatch.hpp Overview
#pragma once
#include <cstdint>

namespace sjsu
{
class StopWatch
{
 public:
  enum class DeltaScale
  {
    kSecond,
    kMicrosecond,
    kNanosecond
  };

  explicit StopWatch(uint32_t (*ticks)())
      : tick_(ticks),
        calibrate_delta_(0),
        start_ticks_(0)
  {
  }

  void Calibrate()
  {
    calibrate_delta_ = 0;
    Start();
    calibrate_delta_ = Stop();
  }

  // Gets the cycle counts
  uint32_t CurrentTicks()
  {
    return tick_();
  }

  void Start()
  {
    start_ticks_ = CurrentTicks();
  }

  uint32_t Stop()
  {
    return (CurrentTicks() - start_ticks_) - calibrate_delta_;
  }

 private:
  uint32_t (*tick_)();
  uint32_t calibrate_delta_;
  uint32_t start_ticks_;
};

}  // namespace sjsu


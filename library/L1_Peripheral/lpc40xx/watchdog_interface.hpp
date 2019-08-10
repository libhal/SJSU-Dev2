#pragma once

#include <cstdint>
#include <iterator>

namespace sjsu
{

class WatchdogInterface
{
 public:
     virtual void Initialize(uint32_t timer_constant, uint32_t timer_window,
     uint32_t timer_warning) = 0;
     virtual void RegisterIrqHandler() = 0;
     virtual void FeedSequence() = 0;
     virtual uint32_t CheckTimeout() = 0;
};

}  // namespace sjsu

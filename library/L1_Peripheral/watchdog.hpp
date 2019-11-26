#pragma once

#include <cstdint>
#include "third_party/units/units.h"
#include "utility/status.hpp"

namespace sjsu
{
class Watchdog
{
 public:
  virtual Status Initialize(std::chrono::seconds duration) const = 0;
  virtual void Enable() const                                    = 0;
  virtual void FeedSequence() const                              = 0;
  virtual uint32_t CheckTimeout() const                          = 0;
};
}  // namespace sjsu

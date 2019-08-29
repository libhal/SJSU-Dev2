#pragma once

#include "utility/status.hpp"
#include "utility/units.hpp"
#include "third_party/result/result.h"

namespace sjsu
{
class Temperature
{
 public:
  virtual Status Initialize()                                            = 0;
  virtual Result<units::temperature::celsius_t, Status> GetTemperature() = 0;
};
}  // namespace sjsu

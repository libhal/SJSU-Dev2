#pragma once

#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
class Temperature
{
 public:
  virtual Status Initialize() = 0;
  virtual Status GetTemperature(
      units::temperature::celsius_t * temperature) = 0;
};
}  // namespace sjsu

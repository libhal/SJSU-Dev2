// @ingroup SJSU-Dev2
// @defgroup Distance sensor interface
// @brief This interface contains the methods that all distance sensing devices
// must encompass
// @{
#pragma once

#include <cstdint>
#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
class DistanceSensor
{
 public:
  virtual Status Initialize() = 0;
  virtual Status GetDistance(units::length::millimeter_t * distance) = 0;
  virtual Status GetSignalStrengthPercent(float * strength)          = 0;
};
}  // namespace sjsu

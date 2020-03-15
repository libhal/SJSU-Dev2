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
/// Interface for a sensor that can measure distance in a single dimension, such
/// as 1D lidar, ultrasonic range sensor, or infared distance sensor.
/// @ingroup sensors
class DistanceSensor
{
 public:
  /// Initialize distance sensor hardware. Must be called before running any
  /// other methods.
  virtual Status Initialize() = 0;

  /// Trigger a capture of the current distance reading and return it.
  ///
  /// @param distance - output parameter to contain the distance results.
  virtual Status GetDistance(units::length::millimeter_t * distance) = 0;

  /// Get the signal strength from the distance sensor.
  ///
  /// @param strength - output parameter to contain the strength of the system.
  ///        The exact value
  /// @return Status
  virtual Status GetSignalStrengthPercent(float * strength) = 0;
};
}  // namespace sjsu

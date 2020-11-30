// @ingroup SJSU-Dev2
// @defgroup Distance sensor interface
// @brief This interface contains the methods that all distance sensing devices
// must encompass
// @{
#pragma once

#include <cstdint>

#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// Interface for a sensor that can measure distance in a single dimension, such
/// as 1D lidar, ultrasonic range sensor, or infared distance sensor.
/// @ingroup sensors
class DistanceSensor : public Module
{
 public:
  /// Trigger a capture of the current distance reading and return it.
  ///
  /// @return measured distance
  virtual units::length::millimeter_t GetDistance() = 0;

  /// Get the signal strength from the distance sensor.
  ///
  /// @return the strength of the signal the strength of the measurement.
  virtual float GetSignalStrengthPercent() = 0;
};
}  // namespace sjsu

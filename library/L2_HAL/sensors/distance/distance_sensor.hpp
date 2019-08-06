// @ingroup SJSU-Dev2
// @defgroup Distance sensor interface
// @brief This interface contains the methods that all distance sensing devices
// must encompass
// @{
#pragma once

#include <cstdint>
#include "utility/status.hpp"

namespace sjsu
{
class DistanceSensor
{
 public:
  static constexpr float kConversionCM   = 10;
  static constexpr float kConversionInch = 25.4f;
  static constexpr float kConversionFt   = 304.8f;

  // ==============================
  // Required Methods for Subclasses
  // ==============================
  virtual Status Initialize() const = 0;
  // Distance units are to be in milimeters
  virtual Status GetDistance(uint32_t * distance) const           = 0;
  virtual Status GetSignalStrengthPercent(float * strength) const = 0;

  // ===============
  // Utility Methods
  // ===============
  Status GetDistanceCm(float * distance) const
  {
    uint32_t sensor_reading;
    Status sensor_status;
    sensor_status = GetDistance(&sensor_reading);
    *distance     = static_cast<float>(sensor_reading / kConversionCM);
    return sensor_status;
  }
  Status GetDistanceInch(float * distance) const
  {
    uint32_t sensor_reading;
    Status sensor_status;
    sensor_status = GetDistance(&sensor_reading);
    *distance     = static_cast<float>(sensor_reading / kConversionInch);
    return sensor_status;
  }
  Status GetDistanceFt(float * distance) const
  {
    uint32_t sensor_reading;
    Status sensor_status;
    sensor_status = GetDistance(&sensor_reading);
    *distance     = static_cast<float>(sensor_reading / kConversionFt);
    return sensor_status;
  }
};
}  // namespace sjsu

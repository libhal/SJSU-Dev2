#pragma once

#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// An abstract interface for light sensing device drivers.
class LightSensor
{
 public:
  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  ///
  /// @return The initialization status.
  virtual Status Initialize() const = 0;
  /// @return The sensor reading in units of lux.
  virtual units::illuminance::lux_t GetIlluminance() const = 0;
  /// @return The maximum illuminance reading supported by the device.
  virtual units::illuminance::lux_t GetMaxIlluminance() const = 0;

  // ==============================
  // Utility Methods
  // ==============================

  /// Retreive the detected brightness as a percentage where:
  /// percentage = illuminance / max_illuminance
  ///
  /// @return The illuminance percentage ranging from 0.0f to 1.0f.
  float GetPercentageBrightness() const
  {
    return (GetIlluminance() / GetMaxIlluminance()).to<float>();
  }
};
}  // namespace sjsu

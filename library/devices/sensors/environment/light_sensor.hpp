#pragma once

#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/math/units.hpp"

namespace sjsu
{
/// An abstract interface for light sensing device drivers.
class LightSensor : public Module<>
{
 public:
  /// @return The sensor reading in units of lux.
  virtual units::illuminance::lux_t GetIlluminance() = 0;

  /// @return The maximum illuminance reading supported by the device.
  virtual units::illuminance::lux_t GetMaxIlluminance() = 0;

  // ===========================================================================
  // Helper Functions
  // ===========================================================================

  /// Retreive the detected brightness as a percentage where:
  /// percentage = illuminance / max_illuminance
  ///
  /// @return The illuminance percentage ranging from 0.0f to 1.0f.
  float GetPercentageBrightness()
  {
    return (GetIlluminance() / GetMaxIlluminance()).to<float>();
  }
};
}  // namespace sjsu

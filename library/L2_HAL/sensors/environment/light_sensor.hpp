#pragma once

#include "utility/units.hpp"

namespace sjsu
{
class LightSensor
{
 public:
  virtual bool Initialize() const                             = 0;
  virtual units::illuminance::lux_t GetIlluminance() const    = 0;
  virtual float GetPercentageBrightness() const               = 0;
  virtual units::illuminance::lux_t GetMaxIlluminance() const = 0;
};
}  // namespace sjsu

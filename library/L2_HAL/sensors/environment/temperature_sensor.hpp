#pragma once

#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// An abstract interface for temperature sensing device drivers.
class TemperatureSensor
{
 public:
  /// Initialize peripherals. This must be called before any other
  /// method in this interface is called.
  ///
  /// @return Error_t if an error occurred during initialization
  virtual Returns<void> Initialize() const = 0;

  /// Call this after Initialize to enable the sensor.
  ///
  /// @return Error_t if an error occurred when attempting to enable the device.
  virtual Returns<void> Enable() const = 0;

  /// Retrieves the temperature reading and writes the value to the designated
  /// memory address.
  ///
  /// @return Returns units::temperature::celsius_t on success. On error, will
  //          return Error_t.
  virtual Returns<units::temperature::celsius_t> GetTemperature() const = 0;
};
}  // namespace sjsu

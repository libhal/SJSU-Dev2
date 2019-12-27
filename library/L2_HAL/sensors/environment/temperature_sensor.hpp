#pragma once

#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// An abstract interface for temperature sensing device drivers.
class TemperatureSensor
{
 public:
  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  ///
  /// @return The initialization status.
  virtual Status Initialize() const = 0;
  /// Retrieves the temperature reading and writes the value to the designated
  /// memory address.
  ///
  /// @param temperature Output parameter.
  /// @return Returns Status::kSuccess if the temperature measurement was
  ///         successfully obtained.
  virtual Status GetTemperature(
      units::temperature::celsius_t * temperature) const = 0;
};
}  // namespace sjsu

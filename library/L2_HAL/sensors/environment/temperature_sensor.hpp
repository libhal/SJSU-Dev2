#pragma once

#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// An abstract interface for temperature sensing device drivers.
class TemperatureSensor : public Module
{
 public:
  /// Retrieves the temperature reading and writes the value to the designated
  /// memory address.
  ///
  /// @return Returns units::temperature::celsius_t on success. On error, will
  //          return Error_t.
  virtual units::temperature::celsius_t GetTemperature() = 0;
};
}  // namespace sjsu

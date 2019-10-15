#pragma once

#include <cstdint>

#include "utility/status.hpp"

namespace sjsu
{
class Dac
{
 public:
  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  virtual Status Initialize() const = 0;
  /// Set the DAC output the the value supplied.
  virtual void Write(uint32_t output) const = 0;
  /// Set the DAC to the voltage supplied.
  /// If the voltage is above or below the DAC's voltage range, then the output
  /// will be capped at those voltages.
  virtual void SetVoltage(float voltage) const = 0;
  /// @return number of active bits for the DAC.
  virtual uint8_t GetActiveBits() const = 0;
};
}  // namespace sjsu

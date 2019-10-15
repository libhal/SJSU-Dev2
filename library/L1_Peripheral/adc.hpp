#pragma once

#include <cstdint>

#include "utility/status.hpp"

namespace sjsu
{
/// Common abstraction interface for Analog-to-Digital (ADC) Converter. These
/// peripherals are used to sense a voltage and convert it to a numeric value.
class Adc
{
 public:
  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  virtual Status Initialize() const = 0;
  /// Read the analog signal's value.
  /// The number active bits depends on the ADC being used and be known by
  /// running the GetActiveBits().
  ///
  /// @return Returns the digital representation of the analog.
  virtual uint32_t Read() const = 0;
  /// @return number of active bits for the ADC.
  virtual uint8_t GetActiveBits() const = 0;
};
}  // namespace sjsu

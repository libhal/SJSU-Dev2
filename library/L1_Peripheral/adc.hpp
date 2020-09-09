#pragma once

#include <cstdint>

#include "utility/status.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// Common abstraction interface for Analog-to-Digital (ADC) Converter. These
/// peripherals are used to sense a voltage and convert it to a numeric value.
/// @ingroup l1_peripheral
class Adc
{
 public:
  /// Initialize and enable hardware. This must be called before any other
  /// method in this interface is called.
  virtual Returns<void> Initialize() const = 0;

  /// Read the analog signal's value.
  /// The number active bits depends on the ADC being used and be known by
  /// running the GetActiveBits().
  ///
  /// @returns The digital representation of the analog.
  virtual uint32_t Read() const = 0;

  /// @returns The number of active bits for the ADC.
  virtual uint8_t GetActiveBits() const = 0;

  /// @returns The ADC reference voltage.
  virtual units::voltage::microvolt_t ReferenceVoltage() const = 0;

  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// @returns The ADC resolution based on the active bits.
  uint32_t GetMaximumValue() const
  {
    return (1 << GetActiveBits()) - 1;
  }

  /// Utility method to convert and return the ADC result in voltage using the
  /// following equation:
  ///
  /// voltage = adc_output * adc_reference_voltage / adc_resolution
  ///
  /// @returns The measured voltage.
  units::voltage::microvolt_t Voltage() const
  {
    return Read() * ReferenceVoltage() / GetMaximumValue();
  }
};
}  // namespace sjsu

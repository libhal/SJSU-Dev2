#pragma once

#include <cstdint>

#include "module.hpp"
#include "inactive.hpp"
#include "utility/error_handling.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// Common abstraction interface for Analog-to-Digital (ADC) Converter. These
/// peripherals are used to sense a voltage and convert it to a numeric value.
///
/// @ingroup l1_peripheral
class Adc : public Module
{
 public:
  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  // ---------------------------------------------------------------------------
  // Configuration Methods
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Usage Methods
  // ---------------------------------------------------------------------------

  /// Read the analog signal's value.
  /// The number active bits depends on the ADC being used and be known by
  /// running the GetActiveBits().
  ///
  /// @returns The digital representation of the analog.
  virtual uint32_t Read() = 0;

  /// @returns The number of active bits for the ADC.
  virtual uint8_t GetActiveBits() = 0;

  /// @returns The ADC reference voltage.
  virtual units::voltage::microvolt_t ReferenceVoltage() = 0;

  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// @returns The ADC resolution based on the active bits.
  uint32_t GetMaximumValue()
  {
    return (1 << GetActiveBits()) - 1;
  }

  /// Utility method to convert and return the ADC result in voltage using the
  /// following equation:
  ///
  /// voltage = adc_output * adc_reference_voltage / adc_resolution
  ///
  /// @returns The measured voltage.
  units::voltage::microvolt_t Voltage()
  {
    return Read() * ReferenceVoltage() / GetMaximumValue();
  }
};

/// Template specialization that generates an inactive sjsu::Adc.
template <>
inline sjsu::Adc & GetInactive<sjsu::Adc>()
{
  class InactiveAdc : public sjsu::Adc
  {
   public:
    void ModuleInitialize() override {}
    void ModuleEnable(bool = true) override {}
    uint32_t Read() override
    {
      return 0;
    }
    uint8_t GetActiveBits() override
    {
      return 12;
    }
    units::voltage::microvolt_t ReferenceVoltage() override
    {
      return 0_V;
    }
  };

  static InactiveAdc inactive;
  return inactive;
}
}  // namespace sjsu

#pragma once

#include <cstdint>

#include "inactive.hpp"
#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/math/units.hpp"

namespace sjsu
{
/// Generic settings for a standard ADC peripheral
struct AdcSettings_t
{
  /// The high side voltage reference of the ADC. Used to calculate an
  /// approximation of the voltage of the ADC.
  units::voltage::microvolt_t reference_voltage = 3.3_V;
};

/// Common abstraction interface for Analog-to-Digital (ADC) Converter. These
/// peripherals are used to sense a voltage and convert it to a numeric value.
///
/// @ingroup l1_peripheral
class Adc : public Module<AdcSettings_t>
{
 public:
  /// Read the analog signal's value.
  /// The number active bits depends on the ADC being used and be known by
  /// running the GetActiveBits().
  ///
  /// @returns The digital representation of the analog.
  virtual uint32_t Read() = 0;

  /// @returns The number of active bits for the ADC.
  virtual uint8_t GetActiveBits() = 0;

  // ===========================================================================
  // Helper Functions
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
    return Read() * (CurrentSettings().reference_voltage / GetMaximumValue());
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
    uint32_t Read() override
    {
      return 0;
    }
    uint8_t GetActiveBits() override
    {
      return 12;
    }
  };

  static InactiveAdc inactive;
  return inactive;
}
}  // namespace sjsu

#pragma once

#include <cstdint>

#include "inactive.hpp"
#include "module.hpp"
#include "utility/error_handling.hpp"
#include "utility/math/units.hpp"

namespace sjsu
{
/// An abstract interface for hardware that can generate an analog voltage,
/// typically called a Digital-to-Analog peripheral.
///
/// @ingroup l1_peripheral
class Dac : public Module<>
{
 public:
  /// Set the DAC output the the value supplied. If the value is above what this
  /// driver can support, the value is clamped.
  ///
  /// @param output - what value to write to the DAC register.
  virtual void Write(uint32_t output) = 0;

  /// Set the DAC to the voltage supplied.
  /// If the voltage is above or below the voltage range, then the output
  /// will be capped at those voltages.
  ///
  /// @param voltage - The specific voltage to set the DAC to.
  virtual void SetVoltage(units::voltage::microvolt_t voltage) = 0;

  /// @return number of active bits for the DAC.
  virtual uint8_t GetActiveBits() = 0;
};

/// Template specialization that generates an inactive sjsu::Dac.
template <>
inline sjsu::Dac & GetInactive<sjsu::Dac>()
{
  class InactiveDac : public sjsu::Dac
  {
   public:
    void ModuleInitialize() override {}
    void Write(uint32_t) override {}
    void SetVoltage(units::voltage::microvolt_t) override {}
    uint8_t GetActiveBits() override
    {
      return 12;
    }
  };

  static InactiveDac inactive;
  return inactive;
}
}  // namespace sjsu

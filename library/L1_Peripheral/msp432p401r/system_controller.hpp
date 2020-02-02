#pragma once

#include <cstdint>

#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "L1_Peripheral/system_controller.hpp"
#include "utility/bit.hpp"
// TODO(#): Fully implement the system controller.
namespace sjsu
{
namespace msp432p401r
{
/// System controller for MSP423P401R that controls clock sources, clock speed,
/// clock outputs control, and peripheral clocks.
class SystemController final : public sjsu::SystemController
{
 public:
  void SetSystemClockFrequency(units::frequency::megahertz_t) const override {}

  void SetPeripheralClockDivider(const PeripheralID &, uint8_t) const override
  {
  }

  uint32_t GetPeripheralClockDivider(const PeripheralID &) const override
  {
    return 1;
  }

  units::frequency::hertz_t GetSystemFrequency() const override
  {
    return 3_MHz;
  }

  bool IsPeripheralPoweredUp(const PeripheralID &) const override
  {
    return false;
  }

  void PowerUpPeripheral(const PeripheralID &) const override
  {
  }

  void PowerDownPeripheral(const PeripheralID &) const override
  {
  }
};
}  // namespace msp432p401r
}  // namespace sjsu

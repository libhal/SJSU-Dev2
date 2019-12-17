#pragma once

#include <cstdint>
#include "L1_Peripheral/system_controller.hpp"

namespace sjsu
{
namespace stm32f10x
{
/// System controller for stm32f10x that controls clock sources, clock speed,
/// clock outputs control, and peripheral enabling
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
    return 8_MHz;
  }

  /// Check if a peripheral is powered up by checking the power connection
  /// register. Should typically only be used for unit testing code and
  /// debugging.
  bool IsPeripheralPoweredUp(const PeripheralID &) const override
  {
    return true;
  }
  void PowerUpPeripheral(const PeripheralID &) const override {}
  void PowerDownPeripheral(const PeripheralID &) const override {}
};
}  // namespace stm32f10x
}  // namespace sjsu

#pragma once

#include <cstddef>
#include <cstdint>

namespace sjsu
{
class SystemController
{
 public:
  class PeripheralID
  {
   public:
    uint8_t device_id = -1;
  };
  template <size_t kDeviceId>
  class AddPeripheralID : public PeripheralID
  {
   public:
    constexpr AddPeripheralID()
    {
      device_id = kDeviceId;
    }
  };
  virtual uint32_t SetClockFrequency(uint8_t frequency_in_mhz) const       = 0;
  virtual void SetPeripheralClockDivider(uint8_t peripheral_divider) const = 0;
  virtual uint32_t GetPeripheralClockDivider() const                       = 0;
  virtual uint32_t GetSystemFrequency() const                              = 0;
  virtual uint32_t GetPeripheralFrequency() const                          = 0;
  virtual bool IsPeripheralPoweredUp(
      const PeripheralID & peripheral_select) const = 0;
  virtual void PowerUpPeripheral(
      const PeripheralID & peripheral_select) const = 0;
  virtual void PowerDownPeripheral(
      const PeripheralID & peripheral_select) const = 0;
};
}  // namespace sjsu

#pragma once

#include <cstddef>
#include <cstdint>

namespace sjsu
{
class SystemController
{
 public:
  // ==============================
  // Interface Defintions
  // ==============================

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

  // ==============================
  // Interface Methods
  // ==============================

  /// Control PLLs and dividers to set the system clock rate
  ///
  /// @returns the difference between the frequency supplied and the frequency
  ///          achieved.
  virtual uint32_t SetSystemClockFrequency(uint8_t frequency_in_mhz) const = 0;
  /// Set the peripheral/bus clock frequency divider
  virtual void SetPeripheralClockDivider(const PeripheralID &,
                                         uint8_t peripheral_divider) const = 0;
  /// @return peripheral clock divider
  virtual uint32_t GetPeripheralClockDivider(const PeripheralID &) const = 0;
  /// @return system clock frequency
  virtual uint32_t GetSystemFrequency() const = 0;
  /// Checks hardware and determines if the peripheral is powered up
  virtual bool IsPeripheralPoweredUp(
      const PeripheralID & peripheral_select) const = 0;
  /// Powers up the supplied peripheral
  virtual void PowerUpPeripheral(
      const PeripheralID & peripheral_select) const = 0;
  /// Powers down the supplied peripheral
  virtual void PowerDownPeripheral(
      const PeripheralID & peripheral_select) const = 0;

  // ==============================
  // Utility Methods
  // ==============================

  /// @returns current bus/peripheral operating frequency
  uint32_t GetPeripheralFrequency(const PeripheralID & peripheral_select) const
  {
    uint32_t peripheral_clock_divider =
        GetPeripheralClockDivider(peripheral_select);
    uint32_t result = 0;  // return 0 if peripheral_clock_divider == 0
    if (peripheral_clock_divider != 0)
    {
      result = GetSystemFrequency() / peripheral_clock_divider;
    }
    return result;
  }
};
}  // namespace sjsu

#pragma once

#include <cstddef>
#include <cstdint>

#include "utility/units.hpp"

namespace sjsu
{
/// Abstract interface for a platform's System Controller.
/// A System controller manages a platforms:
///     - Clocks and their speeds
///     - PLLs feeding into the system's clocks
///     - Manages power systems of peripherals and system blocks
///
/// The System controller also gives information about the system such as, what
/// speed is a peripheral running at so that their peripheral drivers can
/// calculate things such as the clock rates for serial communicate or the clock
/// cycles before an event or interrupt is planned to occur.
/// @ingroup l1_peripheral
class SystemController
{
 private:
  /// Global platform system controller scoped within this class. Most
  /// systems only need a single platform system controller, and thus this
  /// can hold a general/default platform system controller that can be
  /// retrieved via SetPlatformController and GetPlatformController.
  static inline SystemController * platform_system_controller = nullptr;

 public:
  /// Set the controller for the platform. This is set by the system's
  /// L0_Platform startup code which and does not need to be executed by the
  /// user. This can be run by the user if they want to inject their own
  /// Platform Controller into the system to be used by the whole system.
  ///
  /// @param system_controller - a pointer to the current platform's
  ///        system controller.
  static void SetPlatformController(SystemController * system_controller)
  {
    platform_system_controller = system_controller;
  }

  /// Retrieve a reference of the platforms system controller
  static sjsu::SystemController & GetPlatformController()
  {
    return *platform_system_controller;
  }

  // ===========================================================================
  // Interface Defintions
  // ===========================================================================

  /// Peripheral ID is a base class that represents an ID associated with a
  /// peripheral. That Peripheral ID association is used for methods to
  /// determine which peripheral needs to be powered up/down.
  struct PeripheralID  // NOLINT
  {
    /// id associated with the peripheral defined for this object
    uint8_t device_id = -1;
  };

  /// This helper class is used to make it easier to create additional
  /// PeripheralIDs.
  ///
  ///  Usage:
  ///
  ///    inline const auto kUart0 = AddPeripheralID<5>();
  ///    inline const auto kUart1 = AddPeripheralID<6>();
  ///    ...
  ///
  /// Typically the ID number used has some mapping to a register offset or bit
  /// offset within a register. For example, if we have a register for powering
  /// on peripherals and the 5th bit is for Uart0 then the ID for kUart0 should
  /// be 5. In many systems the mapping between clocks and other required things
  /// for a particular peripheral have the same numeric mapping where kUart's
  /// system clock register may be 5 x 4 bytes from the first register handling
  /// clock speeds. This may not always be true, and if not, a look table may be
  /// required.
  ///
  /// @tparam kDeviceId - compile time constant device ID for the peripheral
  template <size_t kDeviceId>
  class AddPeripheralID : public PeripheralID
  {
   public:
    constexpr AddPeripheralID()
    {
      device_id = kDeviceId;
    }
  };

  // ==========================================================================
  // Interface Methods
  // ===========================================================================

  /// Control PLLs and dividers to set the system clock rate
  ///
  /// @param frequency_in_mhz - Set the main system frequency to this frequency
  virtual void SetSystemClockFrequency(
      units::frequency::megahertz_t frequency_in_mhz) const = 0;

  /// Set the peripheral or bus clock frequency divider
  ///
  /// @param id - the peripheral that will have its clock divider changed
  /// @param peripheral_divider - how much to divide the peripheral's clock.
  ///        typically must be a power of 2.
  virtual void SetPeripheralClockDivider(const PeripheralID & id,
                                         uint8_t peripheral_divider) const = 0;

  /// Get the peripheral's clock divider constant
  ///
  /// @param id - peripheral to retrieve the clock divider from
  /// @return the clock divider constant
  virtual uint32_t GetPeripheralClockDivider(const PeripheralID & id) const = 0;

  /// @return system clock's current frequency
  virtual units::frequency::hertz_t GetSystemFrequency() const = 0;

  /// Checks hardware and determines if the peripheral is powered up
  ///
  /// @param peripheral_select - which peripheral to check
  /// @return true - it is currently powered up
  /// @return false - it is currently powered down
  virtual bool IsPeripheralPoweredUp(
      const PeripheralID & peripheral_select) const = 0;

  /// Powers up the selected peripheral
  ///
  /// @param peripheral_select - which peripheral to power up
  virtual void PowerUpPeripheral(
      const PeripheralID & peripheral_select) const = 0;

  /// Powers down the selected peripheral
  ///
  /// @param peripheral_select - which peripheral to power down
  virtual void PowerDownPeripheral(
      const PeripheralID & peripheral_select) const = 0;

  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// @returns current bus/peripheral operating frequency
  ///
  /// @param peripheral_select - which peripheral to calculate the operating
  ///        frequency of.
  units::frequency::hertz_t GetPeripheralFrequency(
      const PeripheralID & peripheral_select) const
  {
    uint32_t peripheral_clock_divider =
        GetPeripheralClockDivider(peripheral_select);
    // return 0 if peripheral_clock_divider == 0
    units::frequency::hertz_t result = 0_Hz;
    if (peripheral_clock_divider != 0)
    {
      result = GetSystemFrequency() / peripheral_clock_divider;
    }
    return result;
  }
};
}  // namespace sjsu

#pragma once

#include <cstddef>
#include <cstdint>

#include "L1_Peripheral/inactive.hpp"
#include "utility/error_handling.hpp"
#include "utility/units.hpp"

namespace sjsu
{
/// Abstract interface for a platform's System Controller.
///
/// A System controller manages a platform's:
///     - Clocks and their speeds
///     - PLLs feeding into the system's clocks
///     - Peripheral and system resource power systems
///
/// The System controller also gives information about the system such as what
/// speed a clock is running at so that their peripheral drivers can calculate
/// things such as the clock rates for serial communicate or the clock cycles
/// before an event or interrupt is planned to occur.
///
/// Every SystemController should take a custom ClockConfiguration data
/// structure reference as an input in their constructor, unless a system does
/// not incorporate any form of clock tree.
///
/// A simplified example of a basic ClockConfiguration data structure. Notice
/// that man of the elements within the structure are enumerations to make them
/// readable:
///
///     struct ClockConfiguration
///     {
///       Oscillator oscillator = Oscillator::kIrc;
///       ClockDividers peripheral_divider = ClockDividers::kDivideBy1;
///       ClockDividers cpu_divider = ClockDividers::kDivideBy1;
///       // ...
///     };
///
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
  /// L0_Platform startup code and does not need to be executed by the
  /// user. This can be run by the user if they want to inject their own
  /// system controller into the system to be used by the whole system.
  ///
  /// @param system_controller - a pointer to the current platform's
  ///        system controller.
  static void SetPlatformController(SystemController * system_controller)
  {
    platform_system_controller = system_controller;
  }

  /// Get the system controller set for this platform.
  /// After main() is called by the startup code, this function will return a
  /// valid system controller. It is required that each platform startup routine
  /// set a system controller using the `void Set(SystemController&)` static
  /// method.
  static SystemController & GetPlatformController()
  {
    return *platform_system_controller;
  }

  // ===========================================================================
  // Interface Defintions
  // ===========================================================================

  /// ResourceID is a base class that represents an ID associated with a
  /// resource managed by the SystemController. That ID association may be used
  /// for methods to determine which peripheral needs to be powered up/down or
  /// to obtain the clock rate of a resource.
  struct ResourceID  // NOLINT
  {
    /// This helper function is used to make it easier to create additional
    /// ResourceID's.
    ///
    ///  Usage:
    ///
    ///    static constexpr auto kUart0 = ResourceID::Define<5>();
    ///    static constexpr auto kUart1 = ResourceID::Define<6>();
    ///    ...
    ///
    /// Typically the ID number used has some mapping to a register offset or
    /// bit offset within a register. For example, if we have a register for
    /// powering on peripherals and the 5th bit is for Uart0 then the ID for
    /// kUart0 should be 5. In many systems the mapping between clocks and other
    /// required things for a particular peripheral have the same numeric
    /// mapping where kUart's system clock register may be 5 x 4 bytes from the
    /// first register handling clock speeds. This may not always be true, and
    /// if not, a look table may be required to map IDs to the appropriate
    /// registers.
    ///
    /// @tparam id - compile time constant device ID for the peripheral
    template <size_t id>
    static constexpr ResourceID Define()
    {
      return { .device_id = id };
    }
    /// ID associated with the resource defined for this object.
    uint32_t device_id = -1;

    /// @param compare - the other resource to compare to this one to.
    /// @return true if their device_id's are equal.
    bool operator==(const ResourceID & compare) const
    {
      return device_id == compare.device_id;
    }
  };

  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  /// Handles self initialization and clock configuration based on the
  /// ClockConfiguration object passed to the controller's constructor. Like
  /// other initialize methods, this may be called multiple times during the
  /// applications runtime.
  ///
  /// @note Invoking this will return the system's clocks back to their safest
  ///       operating modes. This will power on any systems required to operate
  ///       based on the ClockConfiguration object.
  ///
  /// @attention If configuration of the system clocks is desired, one should
  ///            consult the user manual of the target MCU in use to determine
  ///            the valid clock configuration values that can/should be used.
  ///            The Initialize() method is only responsible for configuring the
  ///            clock system based on configurations in the ClockConfiguration.
  ///            Incorrect configurations may result in a hard fault or cause
  ///            the clock system(s) to supply incorrect clock rate(s).
  virtual void Initialize() = 0;

  /// @returns A pointer to the clock configuration object used to configure
  ///          this system controller.
  virtual void * GetClockConfiguration() = 0;

  /// @returns The clock rate frequency of a clock resource.
  /// @returns 0 MHz when a unknown/invalid ResourceID is specified.
  virtual units::frequency::hertz_t GetClockRate(ResourceID resource) const = 0;

  /// Checks hardware and determines if the peripheral is powered up.
  ///
  /// @param peripheral The peripheral to check.
  /// @returns true if the peripheral is currently powered up.
  /// @returns false if the peripheral is currently powered down.
  virtual bool IsPeripheralPoweredUp(ResourceID peripheral) const = 0;

  /// Powers up the selected peripheral.
  ///
  /// @param peripheral The peripheral to power up.
  virtual void PowerUpPeripheral(ResourceID peripheral) const = 0;

  /// Powers down the selected peripheral.
  ///
  /// @param peripheral The peripheral to power down.
  virtual void PowerDownPeripheral(ResourceID peripheral) const = 0;

  // ===========================================================================
  // Utility Methods
  // ===========================================================================

  /// Returns the clock configuration object as a casted reference of
  /// ClockConfiguration supplied in the template parameter, rather than a void*
  /// that would need to be cast later.
  ///
  /// @tparam ClockConfiguration - the clock configuration data structure to
  ///          cast the output to
  /// @return ClockConfiguration - mutable reference to the clock configuration
  ///         object for this system controller.
  template <class ClockConfiguration>
  ClockConfiguration & GetClockConfiguration()
  {
    return *reinterpret_cast<ClockConfiguration *>(GetClockConfiguration());
  }
};

/// Template specialization that generates an inactive sjsu::SystemController.
template <>
inline sjsu::SystemController & GetInactive<sjsu::SystemController>()
{
  class InactiveSystemController : public sjsu::SystemController
  {
   public:
    void Initialize() override {}
    void * GetClockConfiguration() override
    {
      return nullptr;
    }
    units::frequency::hertz_t GetClockRate(ResourceID) const override
    {
      return 0_Hz;
    }
    bool IsPeripheralPoweredUp(ResourceID) const override
    {
      return false;
    }
    void PowerUpPeripheral(ResourceID) const override {}
    void PowerDownPeripheral(ResourceID) const override {}
  };

  static InactiveSystemController inactive;
  return inactive;
}
}  // namespace sjsu

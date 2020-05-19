# System Controller Interface

- [System Controller Interface](#system-controller-interface)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [Interface](#interface)
  - [Configuring the Default System Controller for the Platform](#configuring-the-default-system-controller-for-the-platform)
    - [static void SetPlatformController(SystemController * system_controller)](#static-void-setplatformcontrollersystemcontroller--systemcontroller)
    - [static SystemController & GetPlatformController()](#static-systemcontroller--getplatformcontroller)
  - [Clock Configuration](#clock-configuration)
    - [void * GetClockConfiguration()](#void--getclockconfiguration)
    - [ClockConfiguration & GetClockConfiguration()](#clockconfiguration--getclockconfiguration)
  - [Initialization & Clock Configuration](#initialization--clock-configuration)
    - [Returns&lt;void> Initialize()](#returnsvoid-initialize)
  - [Getting the Clock Rate of a Resource](#getting-the-clock-rate-of-a-resource)
    - [Returns&lt;units::frequency::hertz_t> GetClockRate(ResourceID resource) const](#returnsunitsfrequencyhertzt-getclockrateresourceid-resource-const)
  - [Peripheral Power Control](#peripheral-power-control)
    - [Returns&lt;bool> IsPeripheralPoweredUp(ResourceID peripheral) const](#returnsbool-isperipheralpoweredupresourceid-peripheral-const)
    - [Returns&lt;void> PowerUpPeripheral(ResourceID peripheral) const](#returnsvoid-powerupperipheralresourceid-peripheral-const)
    - [Returns&lt;void> PowerDownPeripheral(ResourceID peripheral) const](#returnsvoid-powerdownperipheralresourceid-peripheral-const)
- [Future Advancements](#future-advancements)
- [Testing Plan](#testing-plan)
  - [Unit Testing Scheme](#unit-testing-scheme)

# Location
`L1 Peripheral`

# Type
`Interface`

# Background
A SystemController manages a platform's:

  - Clocks and their speeds
  - PLLs feeding into the system's clocks
  - Peripheral and system resource power systems

The SystemController also gives information about the system such as what
speed a clock is running at so that their peripheral drivers can calculate
things such as the clock rates for serial communication or the clock cycles
before an event or interrupt is planned to occur.

# Overview
Every SystemController should take a custom `ClockConfiguration` data
structure reference as an input in their constructor unless a system does
not incorporate any form of clock tree.

> See [Clock Control Guide](https://sjsu-dev2.readthedocs.io/en/latest/guides/clock_systems)
> for more information regarding the use of `ClockConfiguration`.

The SystemController uses the `ResourceID` helper class to define various
peripherals, modules, or resources that are managed by the controller. Typically
the ID number used has some mapping to a register offset or bit offset within a
register. For example, if we have a register for powering on peripherals and the
5th bit is for Uart0 then the ID for `kUart0` should be `5`.

In many systems, such as the LPC17xx and LPC40xx, the mapping between clocks and
other required things for a particular peripheral have the same numeric mapping
where `kUart`'s system clock register may be 5 x 4 bytes from the first register
handling clock speeds. This may not always be true, and if not, a look table may
be required to map IDs to the appropriate registers.

# Detailed Design

## Interface

```C++
class SystemController
{
 private:
  static inline SystemController * platform_system_controller = nullptr;

 public:
  static void SetPlatformController(SystemController * system_controller);
  static SystemController & GetPlatformController();

  struct ResourceID
  {
    template <size_t id>
    static constexpr ResourceID Define();

    uint32_t device_id = -1;

    bool operator==(const ResourceID & compare) const;
  };

  virtual Returns<void> Initialize() = 0;
  virtual void * GetClockConfiguration() = 0;
  virtual Returns<units::frequency::hertz_t> GetClockRate(
      ResourceID resource) const = 0;
  virtual Returns<bool> IsPeripheralPoweredUp(ResourceID peripheral) const = 0;
  virtual Returns<void> PowerUpPeripheral(ResourceID peripheral) const = 0;
  virtual Returns<void> PowerDownPeripheral(ResourceID peripheral) const = 0;

  template <class ClockConfiguration>
  ClockConfiguration & GetClockConfiguration();
};
```

## Configuring the Default System Controller for the Platform

### static void SetPlatformController(SystemController * system_controller)
Sets the specified system controller for the platform. Each platform is
responsible for calling this function correctly from within the
`InitializePlatform()` function. Without doing so, access to
`GetPlatformController()` will return a `nullptr`.

### static SystemController & GetPlatformController()
Returns the current system controller for the platform. This is used by
peripheral drivers to power themselves on and determine their running clock
rate. This is also used for application developers to access and set up any
clock configurations.

!!! Warning
    This should not be used by `L2 HAL` or `L3 Application` as knowledge about
    the particular platform is not and should not be known at those levels.

## Clock Configuration

### void * GetClockConfiguration()
Returns a pointer to the memory location of the system controller's
`ClockConfiguration`. In general this should not be called directly. It is
preferred to call `ClockConfiguration & GetClockConfiguration()` instead.

### ClockConfiguration & GetClockConfiguration()
A shorthand for getting the clock configuration for the system controller of a
specific platform by invoking `void * GetClockConfiguration()` and performing
`reinterpret_cast<ClockConfiguration *>()` on the result to return the specified
`ClockConfiguration`. This function should be used to obtain and modify any
desired clock configurations for the system.

## Initialization & Clock Configuration

### Returns&lt;void> Initialize()
Initializes and configures the system clocks and/or PLLs with the configurations
specified in the `ClockConfiguration`.

## Getting the Clock Rate of a Resource

### Returns&lt;units::frequency::hertz_t> GetClockRate(ResourceID resource) const
Returns the running clock rate of the specified resource.

## Peripheral Power Control

### Returns&lt;bool> IsPeripheralPoweredUp(ResourceID peripheral) const
Returns a boolean of whether the specified peripheral is powered on or off.

### Returns&lt;void> PowerUpPeripheral(ResourceID peripheral) const
Power up the selected peripheral.

### Returns&lt;void> PowerDownPeripheral(ResourceID peripheral) const
Power down the selected peripheral.

# Future Advancements
N/A

# Testing Plan

## Unit Testing Scheme
N/A

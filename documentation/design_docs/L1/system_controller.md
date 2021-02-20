# System Controller Interface

- [System Controller Interface](#system-controller-interface)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [Interface](#interface)
  - [Configuring the Default System Controller for the Platform](#configuring-the-default-system-controller-for-the-platform)
    - [static void SetPlatformController(SystemController * system_controller)](#static-void-setplatformcontrollersystemcontroller--system_controller)
    - [static SystemController & GetPlatformController()](#static-systemcontroller--getplatformcontroller)
  - [Defining System Controller Resources](#defining-system-controller-resources)
  - [Clock Configuration](#clock-configuration)
    - [void * GetClockConfiguration()](#void--getclockconfiguration)
    - [template &lt;class ClockConfiguration> ClockConfiguration & GetClockConfiguration()](#template-class-clockconfiguration-clockconfiguration--getclockconfiguration)
  - [Initialization & Clock Configuration](#initialization--clock-configuration)
    - [void Initialize()](#void-initialize)
  - [Getting the Clock Rate of a Resource](#getting-the-clock-rate-of-a-resource)
    - [units::frequency::hertz_t GetClockRate(ResourceID resource) const](#unitsfrequencyhertz_t-getclockrateresourceid-resource-const)
  - [Peripheral Power Control](#peripheral-power-control)
    - [bool IsPeripheralPoweredUp(ResourceID peripheral) const](#bool-isperipheralpoweredupresourceid-peripheral-const)
    - [void PowerUpPeripheral(ResourceID peripheral) const](#void-powerupperipheralresourceid-peripheral-const)
    - [void PowerDownPeripheral(ResourceID peripheral) const](#void-powerdownperipheralresourceid-peripheral-const)
- [Future Advancements](#future-advancements)
- [Testing Plan](#testing-plan)
  - [Unit Testing Scheme](#unit-testing-scheme)

# Location
`peripherals`

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

  virtual void Initialize() = 0;
  virtual void * GetClockConfiguration() = 0;
  virtual units::frequency::hertz_t GetClockRate(
      ResourceID resource) const = 0;
  virtual bool IsPeripheralPoweredUp(ResourceID peripheral) const = 0;
  virtual void PowerUpPeripheral(ResourceID peripheral) const = 0;
  virtual void PowerDownPeripheral(ResourceID peripheral) const = 0;

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
    This should not be used by `devices` or `systems` as knowledge about
    the particular platform is not and should not be known at those levels.

## Defining System Controller Resources

## Clock Configuration

### void * GetClockConfiguration()
Returns a pointer to the memory location of the system controller's
`ClockConfiguration`. In general this should not be called directly. It is
preferred to call `ClockConfiguration & GetClockConfiguration()` instead.

### template &lt;class ClockConfiguration> ClockConfiguration & GetClockConfiguration()
A shorthand for getting the clock configuration for the system controller of a
specific platform by invoking `void * GetClockConfiguration()` and performing
`reinterpret_cast<ClockConfiguration *>()` on the result to return the specified
`ClockConfiguration`. This function should be used to obtain and modify any
desired clock configurations for the system.

## Initialization & Clock Configuration

### void Initialize()
Initializes and configures the system clocks and/or PLLs with the configurations
specified in the `ClockConfiguration`.

!!! Attention
    The `Initialize()` method shall only be responsible for configuring the
    `SystemController` and its clocks and/or PLLs using the settings provided in
    the `ClockConfiguration`.

    `Initialize()` shall not be responsible for
    verifying the configurations in `ClockConfiguration`. It is the developer's
    responsibility to review and understand the system's configurations through
    the user manual of the target MCU to ensure configurations set in the
    `ClockConfiguration` are valid.

## Getting the Clock Rate of a Resource

### units::frequency::hertz_t GetClockRate(ResourceID resource) const
Returns the running clock rate of the specified resource. `0 Hz` shall be
returned if the specified resource has not been previously defined by
`ResourceID::Define<size_t>()`.

## Peripheral Power Control

### bool IsPeripheralPoweredUp(ResourceID peripheral) const
Returns a boolean of whether the specified peripheral is powered on or off. This
function shall always return `false` if checking the power on status of a
peripheral(s) is not supported.

### void PowerUpPeripheral(ResourceID peripheral) const
Power up the selected peripheral.

### void PowerDownPeripheral(ResourceID peripheral) const
Power down the selected peripheral.

# Future Advancements
N/A

# Testing Plan

## Unit Testing Scheme
N/A

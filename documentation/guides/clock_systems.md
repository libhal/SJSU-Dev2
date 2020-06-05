# Clock Control

In order for systems to operate they need a clock signal. Almost all platforms
have a means of controlling, in some fashion the clock rate supplied to their
internal systems. Systems typically support the following:

1. Selecting which oscillators to use as frequency sources.
2. Dividing/reducing the frequency of clock sources.
3. Multiplying/increasing the frequency of clock sources.
4. Further reduce the frequency of clock sources to specific subsystems and
   peripherals.

This does not describe all systems, but is typical.

Increasing the clock rate of particular system can increase their performance,
but can increase power consumption. Decreasing the clock rate can decrease
performance but will also decrease power consumption.

## How this is handled in SJSU-Dev2

Clocking and power management is handled by the platform's
[`L1/SystemController`](https://github.com/SJSU-Dev2/SJSU-Dev2/blob/master/library/L1_Peripheral/system_controller.hpp).
Clocking system can be quite unique across platforms. So unique that a
functional API for controlling clock systems is not feasible. Thus the
`SystemController` interface only exposes two functions for controlling the
clocks of a system. Those are:

```C++
/// Handles self initialization and clock configuration based on the
/// ClockConfiguration object passed to the controller's constructor. Like other
/// initialize methods, this may be called multiple times during the
/// applications runtime. Note that calling this will return the system's clocks
/// back to their safest operating modes, and will then times This will power
/// on any systems required to operate based on the ClockConfiguration object.
virtual void Initialize() = 0;

/// @returns a pointer to the clock configuration object used to
///          configure this system controller.
virtual void * GetClockConfiguration() = 0;
```

Read the API comments in the code block above to get an idea of what these two
methods do.

## Clock Configuration In Main

At startup, before main is called, the default `InitializePlatform()` will set
the platform's system controller as the global platform controller. This means
you can retrieve a reference to the platform controller using:

```C++
auto & controller = sjsu::SystemController::GetPlatformController();
```

### Example of ClockConfiguration object

Lets use the LPC40xx platform as an example as its clock tree is quite small.
A clock tree is the path and interconnection of clock sources to each other in
the system.

```C++
struct ClockConfiguration
{
 // =========================================================================
 // Oscillator Settings
 // =========================================================================

 /// The frequency of the external oscillator crystal. This field must not be
 /// set to 0_Mhz if an external oscillator is to be used anywhere in the
 /// system.
 units::frequency::megahertz_t external_oscillator_frequency = 0_MHz;

 /// The choice for system oscillator. If the external oscillator is used,
 /// then the `external_oscillator_frequency` must be some non-zero value.
 OscillatorSource system_oscillator = OscillatorSource::kIrc;

 // =========================================================================
 // PLL Configuration
 // =========================================================================

 /// Represents the two PLLs in the clock tree
 struct
 {
   /// Whether the PLL should be enabled or not.
   bool enabled = false;
   /// The output rate multiplication factor of the PLL. If the input is 12
   /// MHz and the multiply constant is 2, then the output frequency will be
   /// 24 MHz. The limit for this can be found in the
   ///    datasheets/sjtwo/LPC40xx/UM10562.pdf
   ///     or
   ///    datasheets/lpc40xx/UM10562.pdf
   uint8_t multiply = 1;
 } pll[2] = {};

 // =========================================================================
 // Clock Sources and Dividers
 // =========================================================================

 /// Represents the configuration options for the CPU
 struct
 {
   CpuClockSelect clock = CpuClockSelect::kSystemClock;
   uint8_t divider      = 1;
 } cpu = {};

 /// Represents the configuration options for USB
 struct
 {
   UsbClockSelect clock = UsbClockSelect::kSystemClock;
   UsbDivider divider   = UsbDivider::kDivideBy1;
 } usb = {};

 /// Represents the configuration options for SPIFI
 struct
 {
   SpifiClockSelect clock = SpifiClockSelect::kSystemClock;
   uint8_t divider        = 1;
 } spifi = {};

 /// Defines the peripheral clock divider amount
 uint8_t peripheral_divider = 1;
 /// Defines the EMC clock divider amount
 EmcDivider emc_divider = EmcDivider::kSameSpeedAsCpu;
};
```

ClockConfiguration objects are POD (plain 'ol data), thus they will not have
methods or even static functions to do certain operations, although this may
change in the future.

Each field in the structure that is not a number or bool is an enumeration used
to constrain the possible options a particular field can be.

!!! Note
    In most cases, the clock tree diagram in the user manual for the platform
    will need to be consulted in order to understand how each of the fields in
    the ClockConfiguration object connects with each other. If an field is
    missing and you need it for your project, please be sure to add a feature
    request issue on the github.

### Clock Configuration Retrieval & Modification

Here we grab the platform controller and store it into a `system` reference
variable. Next we use that reference to get the `ClockConfiguration` using
`GetClockConfiguration()`. This is version of `GetClockConfiguration()` takes
the type of the `ClockConfiguration` object which is used to cast and return the
a reference to that `ClockConfiguration` type rather than `void*`. Notice that
the platform configuration object is typically defined within the
`SystemController`.

```C++
auto & system = sjsu::SystemController::GetPlatformController();
auto & config = system.GetClockConfiguration<
   sjsu::lpc40xx::SystemController::ClockConfiguration>();
```

Now that we have a reference to the config object, we can modify it.

```C++
// Select system clock (use crystal oscillator directly)
config.cpu.clock =
   sjsu::lpc40xx::SystemController::CpuClockSelect::kSystemClock;
// Make sure to utilize the internal oscillator only.
config.system_oscillator =
   sjsu::lpc40xx::SystemController::OscillatorSource::kIrc;
// Make sure PLL0 is enabled and available
config.pll[0].enabled  = true;
config.pll[0].multiply = 4;
```

Finally, in order to actually use these new settings, we have two methods:

#### Method 1

```C++
auto result = system.Initialize();
if (!result)
{
  // Handle error ...
}
```

This option seems like the obvious choice but has a fatal issue. During platform
initialization, peripherals and other drivers that are enabled at startup will
have adjusted their own clock frequencies based on the frequency they were fed
initially. If you change underlying clock without also rerunning those driver's
initialize methods, their clocks will be incorrect and the drivers will not
perform as expected. The behavior is undefined and can potentially lock up the
platform.

#### Method 2 (best practice)

```C++
sjsu::InitializePlatform();
```

Calling the `InitializePlatform()` function directly, will prevent the issues in
Method 1, since `InitializePlatform()` will handle re-initializing the
peripherals based on the new clock settings.

!!! Note
    This only re-initializes the peripherals used at startup. These WILL NOT
    re-initialize all of the peripherals and drivers you have in your
    application.

## Clock Configuration Examples

Clock configuration examples can be found in the `demos/` folder with the name
`clock_configuration`.

- [demos/stm32f10x/clock_configuration](https://github.com/SJSU-Dev2/SJSU-Dev2/tree/master/demos/stm32f10x/clock_configuration)
- [demos/lpc40xx/clock_configuration](https://github.com/SJSU-Dev2/SJSU-Dev2/tree/master/demos/sjtwo/clock_configuration)

## Best practices

### Best time to change clocks

The best time to change the clock rate for the platform is at the beginning of
the main() function before any peripherals or drivers have been initialized.
Prefer to only change the clock rate once during runtime.

Note that if you change the operating frequency of the system after peripherals
or other drivers have already been initialized, then those drivers will have
been setup with a frequency that does not match the actual state of the device.
In this case, you either need to re-run the initialization of each of the
drivers and peripherals in your application or simply set the clocks before
initialization, which tends to reduce code complexity and size. Failure to do
this will lead to undefined behavior.

### Changing clock rates in a thread

First off, DON'T! But if you feel the need to, prefer to have just 1 thread
manage this, or have a mutex lock usage of this resource. The clock tree is a
single hardware resource and the configuration object is a single statically
allocated object, thus are not thread safe to manipulate without some sort of
protection.

### Power Savings

Set the clock rates to the lowest values possible that still fit your
performance needs. This will very likely require testing of the platform to
verify that the platform is performing as expected.

After that point, there is no need to change the clock rates further. If you
want to lower the power consumption further, you will need to put the device
into a low power or sleep mode. Do not adjust the clocks dynamically at runtime
in order to reduce power. Use the platform's low power modes.

### Resetting Configuration Objects

All configuration objects come default initialized to a state that reflects the
reset state of the platform, but in the case where it has been changed and needs
to be reset to a known state, simply creating an instance of that clock
configuration object and assigning it to the config reference will do the trick.

```C++
sjsu::lpc40xx::SystemController::ClockConfiguration new_config;
auto & config = system.GetClockConfiguration<
   sjsu::lpc40xx::SystemController::ClockConfiguration>();

// Set to the new configuration, which will reset the configuration back to its
// default state.
config = new_config;

// Call sjsu::InitializePlatform() to allow the new clocks to take effect.
sjsu::InitializePlatform();
```

## Debugging

- I ran `sjsu::InitializePlatform()` and my platform stopped working, hard
  faulted or crashed.
      - It is likely that the settings you gave to the configuration object are
        outside of the bounds of what is acceptable for the platform, causing
        this issue. Consult the user manual to see if there is anything you have
        done incorrectly.

## Information For Collaborators

1. `sjsu::InitializePlatform()` MUST NOT alter the ClockConfiguration object
   when it is called, to allow the user to have full control over the object.
2. Every field in the `ClockConfiguration` structure must have a default
   value that reflects the reset state of the device.
3. The `sjsu::SystemController::Initialize()` method, like all other
   `Initialize()` methods must be callable multiple times.

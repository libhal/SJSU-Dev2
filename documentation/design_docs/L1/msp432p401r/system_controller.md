# MSP432P4xx System Controller

- [MSP432P4xx System Controller](#msp432p4xx-system-controller)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [API](#api)
  - [Clock System on Reset](#clock-system-on-reset)
  - [Writing Changes to the Clock System registers](#writing-changes-to-the-clock-system-registers)
    - [void UnlockClockSystemRegister() const](#void-unlockclocksystemregister-const)
    - [void LockClockSystemRegister() const](#void-lockclocksystemregister-const)
  - [Initialization](#initialization)
    - [Returns&lt;void> Initialize() override](#returnsvoid-initialize-override)
    - [Returns&lt;units::frequency::hertz_t> ConfigureDcoClock() const](#returnsunitsfrequencyhertz_t-configuredcoclock-const)
    - [Returns&lt;units::frequency::hertz_t> ConfigureReferenceClock() const](#returnsunitsfrequencyhertz_t-configurereferenceclock-const)
    - [Returns&lt;void> SetClockSource(Clock clock, Oscillator oscillator) const](#returnsvoid-setclocksourceclock-clock-oscillator-oscillator-const)
    - [Returns&lt;void> SetClockDivider(Clock clock, ClockDivider divider) const](#returnsvoid-setclockdividerclock-clock-clockdivider-divider-const)
    - [void WaitForClockReadyStatus(Clock clock) const](#void-waitforclockreadystatusclock-clock-const)
  - [Getting the Clock Rate of a Clock Signal](#getting-the-clock-rate-of-a-clock-signal)
    - [Returns&lt;units::frequency::hertz_t> GetClockRate(ResourceID peripheral) const override](#returnsunitsfrequencyhertz_t-getclockrateresourceid-peripheral-const-override)
  - [Unused Functions](#unused-functions)
- [Caveats](#caveats)
- [Future Advancements](#future-advancements)
- [Testing Plan](#testing-plan)
  - [Unit Testing Scheme](#unit-testing-scheme)
    - [Initialize()](#initialize)
    - [GetClockRate(ResourceID peripheral)](#getclockrateresourceid-peripheral)
    - [SetClockDivider(Clock clock, ClockDivider divider)](#setclockdividerclock-clock-clockdivider-divider)
    - [IsPeripheralPoweredUp()](#isperipheralpoweredup)
  - [Demonstration Project](#demonstration-project)

# Location
`peripherals`

# Type
`Implementation`

# Background
The following internal oscillators are available in the clock system module:

1. Low-frequency oscillator (LFXT).
2. High-frequency oscillator (HFXT).
3. Digitally controlled oscillator (DCO).
4. Very low-power low-frequency oscillator (VLO).
5. Low-power low-frequency oscillator (REFO).
6. Low-power oscillator (MODOSC).
7. System oscillator (SYSOSC).

The MSP432P4xx MCUs have five primary clock signals which can be driven by the
oscillators mentioned above to drive the individual peripheral modules:

1. Auxiliary clock (ACLK).
2. Master clock (MCLK) which drives the CPU.
3. Subsystem clock (HSMCLK).
4. Low-speed subsystem clock (SMCLK).
5. Low-speed backup domain clock (BCLK).

> More information regarding the clock system module can be found in the
[Ti MSP432P4xx Technical Reference Manual](https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=378).

# Overview
The system controller allows the configuration of the clock system module. By
default the Master clock and subsequently the CPU clock, shall be driven by the
digitally controlled oscillator (DCOCLK). The DCOCLK can be configured to
produce a target nominal frequency between 1 MHz and 48 MHz. For the SJSU-Dev2
platform, a default CPU clock rate of 48 MHz is used.

# Detailed Design
## API
```c++
class SystemController : public sjsu::SystemController
{
 public:
  enum class Oscillator : uint8_t;
  enum class Clock : uint8_t;
  enum class ClockDivider : uint8_t;

  struct ClockConfiguration_t;

  explicit constexpr SystemController(
      ClockConfiguration_t & clock_configuration);

  void Initialize() override;
  void * GetClockConfiguration() override;
  units::frequency::hertz_t GetClockRate(
     ResourceID peripheral) const override;
  void SetClockDivider(Clock clock, ClockDivider divider) const;

  bool IsPeripheralPoweredUp(ResourceID) const override;
  void PowerUpPeripheral(ResourceID) const override;
  void PowerDownPeripheral(ResourceID) const override;

 private:
  void UnlockClockSystemRegisters() const;
  void LockClockSystemRegisters() const;
  void WaitForClockReadyStatus(Clock clock) const;
  void SetClockSource(Clock clock, Oscillator source) const;
  units::frequency::hertz_t ConfigureDcoClock() const;
  units::frequency::hertz_t ConfigureReferenceClock() const;
};
```

## Clock System on Reset
> See [6.2 Clock System Operation](https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=381).

## Writing Changes to the Clock System registers
The clock system registers are locked and a 16-bit key, `0x695A`, must be
written to the Clock System Key register (CSKEY) to unlock the registers before
any configurations can be made. The registers can be locked again by writing any
other value after the desired configurations are set.

### void UnlockClockSystemRegister() const
Unlocks the clock system registers by writing `0x0695A`.

### void LockClockSystemRegister() const
Locks the clock system registers by writing `0x0000`.

## Initialization

### Returns&lt;void> Initialize() override
Performs the following sequence to configure various clock modules:

1. Configure the DCO clock and reference clock by invoking `ConfigureDcoClock()`
   and `ConfigureReferenceClock()`.
2. Set then clock divider for each of the primary clocks.
3. Set the clock source for the available primary clocks.
4. Determine and store the running clock rates for the primary clocks and the
   reference clock.

An **Error_t** is returned if:

1. ConfigureDcoClock() returns an error.
2. ConfigureReferenceClock() returns an error.
3. The configured clock source for the auxiliary clock is not
   `Oscillator::kLowFrequency`, `Oscillator::kVeryLowFrequency`, or
   `Oscillator::kReference`.
4. The configured clock source for the backup clock is not
   `Oscillator::kLowFrequency` or `Oscillator::kReference`.

### Returns&lt;units::frequency::hertz_t> ConfigureDcoClock() const
Performs the following sequence to configure the DCO clock:

1. Ensure the target frequency is between 1 MHz and 48 MHz.
2. Determine the DCO tuning configuration values by finding the DCO frequency
   range, DCO constant, and DCO calibration values based on the desired target
   frequency.
3. Calculate the signed 10-bit tuning value.
4. Write the configurations to the CSCTL0 register.

> See [6.2.8.3 DCO Ranges and Tuning](https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=386)
> for more information regarding configuring the DCO clock.

An **Error_t** is returned if the target frequency is not between 1 MHz and 48
MHz.

### Returns&lt;units::frequency::hertz_t> ConfigureReferenceClock() const
Configures the reference clock to run at 32.768 kHz or 128 kHz.

An **Error_t** is returned if the frequency select value in the clock
configuration for the reference clock is not `0b0` or `0b1`.

### Returns&lt;void> SetClockSource(Clock clock, Oscillator oscillator) const
Sets the desired clock source to drive a primary clock signal.

An **Error_t** is returned if `clock` is not one of the primary clocks.

### Returns&lt;void> SetClockDivider(Clock clock, ClockDivider divider) const
Sets the desired clock divider for a primary clock signal.

An **Error_t** is returned if `clock` is not one of the primary clocks
(excluding the backup clock).

### void WaitForClockReadyStatus(Clock clock) const
Checks and waits for the specified primary clock signal to become stable before
proceeding. This is necessary when changing the a clock signal's frequency or
divider.

## Getting the Clock Rate of a Clock Signal

### Returns&lt;units::frequency::hertz_t> GetClockRate(ResourceID peripheral) const override
Gets the clock rate of one of the 10 available clock modules.

An **Error_t** is returned if `peripheral` is not one of the defined peripherals
in the `Modules` namespace.

## Unused Functions
The following functions are not implemented or used:

```c++
bool IsPeripheralPoweredUp(const ResourceID &) const override
void PowerUpPeripheral(const ResourceID &) const override
void PowerDownPeripheral(const ResourceID &) const override
```

# Caveats
N/A

# Future Advancements
N/A

# Testing Plan

## Unit Testing Scheme
The following functions shall be tested:

### Initialize()
Each of the internal oscillators shall be used as a clock source to configure
each primary clock. Additionally, clock divider values of 1, 2, 4, 8, 16, 32,
64, and 128 shall be set for each configuration:

1. Configure auxiliary clock:
   - The function should:
     - Set the selected auxiliary clock source select value in the CSCTL1
       register.
     - Set the selected auxiliary clock divider select value in the CSCTL1
       register.
     - Have a running auxiliary clock frequency that is equivalent of the
       selected clock source divided by the selected clock divider.
   - The function should return an error if the clock source is not
     `Oscillator::kLowFrequency`, `Oscillator::kVeryLowFrequency`, or
     `Oscillator::kReference`.
2. Configure master clock:
   - The function should:
     - Set the selected master clock source select value in the CSCTL1 register.
     - Set the selected master clock divider select value in the CSCTL1
       register.
     - Have a running master clock frequency that is equivalent of the selected
       clock source divided by the selected clock divider.
3. Configure subsystem master clocks:
   - The function should:
     - Set the selected subsystem master clock source select value in the CSCTL1
       register.
     - Set the selected subsystem master clock divider select value in the
       CSCTL1 register.
     - Have a running subsystem master clock frequency that is equivalent of the
       selected clock source divided by the selected clock divider.
     - Set the selected low-speed subsystem master clock source select value in
       the CSCTL1 register.
     - Set the selected low-speed subsystem master clock divider select value in
       the CSCTL1 register.
     - Have a running low-speed subsystem master clock frequency that is
       equivalent of the selected clock source divided by the selected clock
       divider.
4. Configure backup clock:
   - The function should:
     - Set the selected backup clock source select value in the CSCTL1 register.
     - Have a running backup clock frequency that is equivalent of the select
       clock source.
   - The function should return an error if the clock source is not
     `Oscillator::kLowFrequency`, or `Oscillator::kReference`.

### GetClockRate(ResourceID peripheral)
The clock rates of clock signals that have a fixed frequency are verified.

1. When `peripheral` = `Modules::kLowFrequencyClock`
   - The function should return 32,768 Hz.
2. When `peripheral` = `Modules::kVeryLowFrequencyClock`
   - The function should return 9,400 Hz.
3. When `peripheral` = `Modules::kReferenceClock` and
   reference clock is configured for 32.768 kHz
   - The function should return 32,768 Hz.
4. When `peripheral` = `Modules::kReferenceClock` and
   reference clock is configured for 128 kHz
   - The function should return 128,000 Hz.
5. When `peripheral` = `Modules::kModuleClock`
   - The function should return 25,000,000 Hz.
6. When `peripheral` = `Modules::kSystemClock`
   - The function should return 5,000,000 Hz.

### SetClockDivider(Clock clock, ClockDivider divider)
Clock divider values of 1, 2, 4, 8, 16, 32, 64, and 128 shall be set for each
of the available primary clocks except for the backup clock.

1. When `divider` = `ClockDivider::kDivideBy1`
   - The function should set a clock divider select value of `0b000`.
2. When `divider` = `ClockDivider::kDivideBy2`
   - The function should set a clock divider select value of `0b001`.
3. When `divider` = `ClockDivider::kDivideBy4`
   - The function should set a clock divider select value of `0b010`.
4. When `divider` = `ClockDivider::kDivideBy8`
   - The function should set a clock divider select value of `0b011`.
5. When `divider` = `ClockDivider::kDivideBy16`
   - The function should set a clock divider select value of `0b100`.
6. When `divider` = `ClockDivider::kDivideBy32`
   - The function should set a clock divider select value of `0b101`.
7. When `divider` = `ClockDivider::kDivideBy64`
   - The function should set a clock divider select value of `0b110`.
8. When `divider` = `ClockDivider::kDivideBy128`
   - The function should set a clock divider select value of `0b111`.

### IsPeripheralPoweredUp()
- Should always return `false`.

## Demonstration Project
An example project using the system controller can be found
[here](/demos/msp432p401r/system_controller/source/main.cpp).
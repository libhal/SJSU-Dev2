# MSP432P401R System Controller

- [MSP432P401R System Controller](#msp432p401r-system-controller)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [API](#api)
  - [Clock Configuration on Reset](#clock-configuration-on-reset)
  - [Configuring Clock Signals](#configuring-clock-signals)
    - [CPU Clock](#cpu-clock)
    - [Peripheral Clock](#peripheral-clock)
  - [Unused Functions](#unused-functions)
- [Caveats](#caveats)
- [Future Advancements](#future-advancements)
- [Testing Plan](#testing-plan)
  - [Unit Testing Scheme](#unit-testing-scheme)
  - [Demonstration Project](#demonstration-project)

# Location
`L1 Peripheral`

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

The MSP432P401R has five primary clock signals which can be driven by the
oscillators mentioned above to drive the individual peripheral modules:

1. Auxiliary clock (ACLK).
2. Master clock (MCLK) which drives the CPU.
3. Subsystem clock (HSMCLK).
4. Low-speed subsystem clock (SMCLK).
5. Low-speed backup domain clock (BCLK).

The primary clocks signals can be configured to drive the CPU clock or a desired
peripheral module. Additionally, the following clock dividers can be selected
for each of the primary clock signals except for BCLK: 1, 2, 4, 8, 16, 32, 64,
and 128.

More information regarding the clock system module can be found in the
[Ti MSP432P4xx Technical Reference Manual](https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=378).

# Overview
The system controller allows the configuration of the clock system module. The
CPU clock shall be driven by the digitally controlled oscillator (DCO). This
oscillator can be configured to produce a target nominal frequency between 1 MHz
and 48 MHz.

# Detailed Design
## API
```c++
class SystemController final : public sjsu::SystemController
{
 public:
  inline static CS_Type * system_controller = msp432p401r::CS;

  void SetSystemClockFrequency(
      units::frequency::megahertz_t frequency) const override;
  void SetPeripheralClockDivider(const PeripheralID & peripheral_select,
                                 uint8_t peripheral_divider) const override;
  uint32_t GetPeripheralClockDivider(
      const PeripheralID & peripheral_select) const override;
  units::frequency::hertz_t GetSystemFrequency() const override;

  bool IsPeripheralPoweredUp(const PeripheralID &) const override;
  void PowerUpPeripheral(const PeripheralID &) const override;
  void PowerDownPeripheral(const PeripheralID &) const override;

 private:
  void UnlockClockSystemRegisters() const;
  void LockClockSystemRegisters() const;
  void WaitForClockReadyStatus(Clock clock) const;
  void SetClockSource(Clock clock, Oscillator source) const;

  inline static units::frequency::hertz_t speed_in_hertz = 3_MHz;
};
```

## Clock Configuration on Reset
See [6.2 Clock System Operation](https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=381)
in the Reference Manual.

## Configuring Clock Signals
The clock system registers are locked and a 16-bit key, `0x695A`, must be
written to the Clock System Key register (CSKEY) to unlock the registers before
any configurations can be made. The registers can be locked again by writing any
other value after the desired configurations are set.

```c++
void UnlockClockSystemRegister() const
```
Unlocks the clock system registers by writing `0x0695A`.

```c++
void LockClockSystemRegister() const
```
Locks the clock system registers by writing `0x0000`.

```c++
void SetClockSource(Clock clock, Oscillator oscillator) const
```
Sets the desired oscillator to drive a primary clock signal. See
[Reference Manual Table 6-5](https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=397)
for available sources for each primary clock signal.

```c++
void WaitForClockReadyStatus(Clock clock) const
```
Checks and waits for a clock signal to become stable after a frequency or
divider configuration.

### CPU Clock
On reset, the digitally controlled clock (DCOCLK) drives the master clock (MCLK)
with a default frequency of 3 MHz. DCOCLK can be configured by adjusting the
tuning values to obtain a target frequency between 1 MHz to 48 MHz.

```c++
void SetSystemClockFrequency(
      units::frequency::megahertz_t frequency) const override
```
Performs the following sequence to configure the master clock:
1. Check the desired `frequency` and assert an error if it is not between 1 MHz
   to 48 MHz.
2. Determine the DCOCLK frequency range based on the specified target frequency
   and calculate the DCOCLK tune value to obtain the target frequency. The
   calculations required for tuning DCOCLK can be found in
   [6.2.8.3 DCO Ranges and Tuning](https://www.ti.com/lit/ug/slau356i/slau356i.pdf#page=386)
   of the Reference Manual.
3. Unlock the clock system registers to allow configuration changes.
4. Write the source select value to the CSCTL1 register to configure DCOCLK as
   the source for MCLK and HSMCLK.
5. Write the DCOCLK tuning range and select values to the CSCTL0 register to
   configure DCOCLK frequency.
6. Lock the clock system registers.
7. Wait for the clock ready status.

### Peripheral Clock
The subsystem master clock (HSMCLK) is driven by the digitally controlled clock
(DCO) and can be used to drive most peripherals. The speed can be adjusted by
configuring through `SetPeripheralClockDivider`.

```c++
void SetPeripheralClockDivider(const PeripheralID & peripheral_select,
                               uint8_t peripheral_divider) const override
```
Sets the clock divider for HSMCLK. The available divider values are: 1, 2, 4, 8,
16, 32, 64, and 128.

```c++
uint32_t GetPeripheralClockDivider(
      const PeripheralID & peripheral_select) const override
```
Returns the current divider value used for HSMCLK.

## Unused Functions
The following functions are not used:

```c++
bool IsPeripheralPoweredUp(const PeripheralID &) const override
void PowerUpPeripheral(const PeripheralID &) const override
void PowerDownPeripheral(const PeripheralID &) const override
```

# Caveats
To be added...

# Future Advancements
N/A

# Testing Plan
To be added...

## Unit Testing Scheme
To be added...

## Demonstration Project
To be added...

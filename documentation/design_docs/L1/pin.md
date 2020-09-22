# Pin Interface

- [Pin Interface](#pin-interface)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview](#overview)
- [Detailed Design](#detailed-design)
  - [Interface](#interface)
  - [Pin Configuration](#pin-configuration)
    - [constexpr Pin(uint8_t port, uint8_t pin)](#constexpr-pinuint8t-port-uint8t-pin)
    - [Returns&lt;void> Initialize() const](#returnsvoid-initialize-const)
    - [Returns&lt;void> SetPinFunction(uint8_t function) const](#returnsvoid-setpinfunctionuint8t-function-const)
    - [Returns&lt;void> SetPull(Resistor resistor) const](#returnsvoid-setpullresistor-resistor-const)
    - [Returns&lt;void> SetAsOpenDrain(bool set_as_open_drain) const](#returnsvoid-setasopendrainbool-setasopendrain-const)
    - [Returns&lt;void> SetAsAnalogMode(bool set_as_analog) const](#returnsvoid-setasanalogmodebool-setasanalog-const)
  - [Utility Functions](#utility-functions)
    - [void PullUp() const](#void-pullup-const)
    - [void PullDown() const](#void-pulldown-const)
    - [void SetFloating() const](#void-setfloating-const)
    - [uint8_t GetPort() const](#uint8t-getport-const)
    - [uint8_t GetPin() const](#uint8t-getpin-const)
- [Future Advancements](#future-advancements)
- [Testing Plan](#testing-plan)
  - [Unit Testing Scheme](#unit-testing-scheme)
    - [void PullUp() const](#void-pullup-const-1)
    - [void PullDown() const](#void-pulldown-const-1)
    - [void SetFloating() const](#void-setfloating-const-1)

# Location
`L1 Peripheral`

# Type
`Interface`

# Background
Pins are a type of electrical contact. Contacts are metal surfaces which can be
pressed or connected to another contact or pin in order to create an electric
circuit.

The pins of a micro-controller (MCU) typically can support more than one
function or mode. This could be allowing the pin to sense analog voltages.
Another could be a mode, such as SPI mode or UART mode, which allows for
communication with other devices.

Not only can a pin's functions be changed but their properties can also be
changed along side these functions. For example, many controllers have internal
pull up (resistor connected to high voltage) or pull down (resistor connected to
ground) resistors to keep the state of the pin at a known voltage when the pin
is not connected to anything.

# Overview
The `Pin` interface abstracts the process of changing the mode and attributes of
a hardware pin on various MCUs.

# Detailed Design

## Interface
```c++
class Pin
{
 public:
  enum class Resistor : uint8_t
  {
    kNone = 0,
    kPullDown,
    kPullUp,
    kRepeater
  };

  constexpr Pin(uint8_t port, uint8_t pin);

  virtual void Initialize() const = 0;
  virtual void SetPinFunction(uint8_t function) const = 0;
  virtual void SetPull(Resistor resistor) const = 0;
  virtual void SetAsOpenDrain(bool set_as_open_drain) const = 0;
  virtual void SetAsAnalogMode(bool set_as_analog) const = 0;

  void PullUp() const;
  void PullDown() const;
  void SetFloating() const;

  uint8_t GetPort() const;
  uint8_t GetPin() const;
};
```

## Pin Configuration

### constexpr Pin(uint8_t port, uint8_t pin)
Constructs the `Pin` object with the specified port and pin identifier.

### Returns&lt;void> Initialize() const
Performs the necessary operations to initialize the hardware pin for use. An
**Error_t** should be returned when the pin fails to initialize.

### Returns&lt;void> SetPinFunction(uint8_t function) const
Configures the pin based on the specified function code. An **Error_t** should
be returned if the specified function code is invalid.

### Returns&lt;void> SetPull(Resistor resistor) const
Configures the pin based on the specified resistor pull. An **Error_t** should
be returned if the specified resistor pull is not supported.

> **NOTE:** Not all MCUs or pins have support pull up/down resistors.

### Returns&lt;void> SetAsOpenDrain(bool set_as_open_drain) const
Configures the pin to be an open drain pin when `set_as_open_drain` is `true`.

> **Note:** Not all MCUs support this hardware feature. If this is the case,
> then this function should be left unimplemented.

### Returns&lt;void> SetAsAnalogMode(bool set_as_analog) const
Enables the pin's analog mode when `set_as_analog` is `true`.

> **Note:** Not all MCUs support the need to set the pin in analog mode. If this
> is the case, then this function should be left unimplemented.

## Utility Functions

### void PullUp() const
A shorthand for invoking `SetPull(Resistor::kPullUp)`.

### void PullDown() const
A shorthand for invoking `SetPull(Resistor::kPulldown)`.

### void SetFloating() const
A shorthand for invoking `SetPull(Resistor::kNone)`.

### uint8_t GetPort() const
Returns the pin's port identifier.

### uint8_t GetPin() const
Returns the pin's pin identifier.

# Future Advancements
N/A

# Testing Plan

## Unit Testing Scheme
The `Pin` interface shall be mocked and the `SetPull` function shall be faked to
verify the invocation of the utility functions `PullUp()`, `PullDown()`,
and `SetFloating()`.

### void PullUp() const
- This function should invoke `SetPull()` with the argument `Resistor::kPullUp`.

### void PullDown() const
- This function should invoke `SetPull()` with the argument
  `Resistor::kPulldown`.

### void SetFloating() const
- This function should invoke `SetPull()` with the argument `Resistor::kNone`.

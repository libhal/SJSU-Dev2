# Level 1 Peripheral Guidelines

## Implementing a Peripheral

Lets say you are writing a GPIO L1 implementation for an atmega328p
microcontroller. You would need to create a file in the following folder:
`library/L1_Peripheral/atmega328p/gpio.hpp` Your code should roughly follow the
layout and format of the other `gpio.hpp` files in other platforms.

Every peripheral in L1 must be const-able, meaning you can write it like this:

``` c++
const Gpio gpio(/* ... */);
```

and be able to call every method within the interface. This helps with
optimizations later and allows them to be passed as const references to objects
in L2_HAL. The implementation must not alter any internal variables, but can
alter external variables via pointer or direct access.

## Adding an Interface

Lets say you want to implement a peripheral that does not currently have an
interface, like an SPIFI interface.

You would need to create a file in the following folder:
`library/L1_Peripheral/spifi.hpp`

After adding the interface at least 1 implementation of this driver for an
actual platform MUST be added.

Finally, after adding both the interface and 1 demonstration of the interface
used within an actual peripheral driver, an example must be created and put here
`library/L1_Peripheral/example/spifi.hpp`.

## Adding a Platform

Adding a new platform is as simple as adding a folder with the name of the
platform and adding a few drivers. The following list of drivers are not
necessary for every platform but are very common across many microcontrollers.
The list is prioritized in order of importance of each peripheral.

1.  SystemController
2.  Pin
3.  Gpio
4.  Uart
5.  Interrupt
6.  Spi
7.  I2c
8.  Pwm
9.  Adc

Honestly the first three are the most important for getting started, but the
rest get you to the point where most HAL and applications would be satisfied.

## Peripheral Inheritance

Peripherals shall ONLY inherit their appropriate abstract interface class.
Otherwise, peripherals shall NOT inherit other implemented classes. If an object
needs to use another peripheral like pin, use composition (HAS-A) rather than
inheritance (IS-A).0-

## Borrowing Drivers from other systems

Sometimes we get lucky and the register mapping between two distinct MCUs in the
same family are the same.

In this case, we can borrow the implementation of other microcontrollers for
another. All you need to do is put that driver's name in the namespace of the
other microcontroller.

The following borrows the LPC40xx ADC driver for use in the LPC17xx platform.

``` c++
#pragma once

#include "L1_Peripheral/lpc40xx/adc.hpp"

namespace sjsu
{
namespace lpc17xx
{
// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Adc;
}  // namespace lpc17xx
}  // namespace sjsu
```

# Testing L1

Testing L1 Peripherals on host side is done via two methods of
dependency injection.

## Using LUT or Single Register Pointer

Take the following example for a GPIO object:

``` c++
class Gpio
{
 public:
  static LPC_GPIO_TypeDef * gpio[] = { LPC_GPIO0, LPC_GPIO1, /* ... */ };
  // ...
  Gpio(uint8_t port, uint8_t pin) : port_(port), pin_(pin)
  {
  }
};
```

We can create our own version of the `LPC_GPIO_TypeDef` structure in our
test and set the entries in the look up table to the address of local
GPIO register description structure.

``` c++
TEST_CASE("Testing Gpio")
{
  // Initialized local LPC_GPIO_TypeDef objects with 0 to observe how the
  // Gpio class manipulates the data in the registers
  LPC_GPIO_TypeDef local_gpio_port[2];
  memset(&local_gpio_port, 0, sizeof(local_gpio_port));
  // Only GPIO port 1 & 2 will be used in this unit test
  Gpio::gpio_port[0] = &local_gpio_port[0];
  Gpio::gpio_port[1] = &local_gpio_port[1];
  // Pins that are to be used in the unit test
  Gpio p0_00(0, 0);
  Gpio p1_07(1, 7);

  // ... Rest of the test code
}
```

After reassigning these addresses you can safely run the Gpio methods, as they
will no longer attempt to access the address on hardware, but will actually
access the data within your local test case. Now you can run your methods and
check if the bits in the structure have been modified in the correct way.

## Using Predefined Structures

This works nearly the same as the LUT/Register pointer testing scheme, except
that you do not need to overwrite and then restore the original values within
the static variable of the object.

``` c++
class Gpio
{
 public:
  static const LPC_GPIO_TypeDef * kPort0[] = LPC_GPIO0;
  static const LPC_GPIO_TypeDef * kPort1[] = LPC_GPIO1;
  /* ... */
  // ...
  Gpio(const LPC_GPIO_TypeDef * port, uint8_t pin) : port_(port), pin_(pin)
  {
  }
};
```

The actual port register address has bene passed into the GPIO object this time.
This is beneficial because all we need to do is pass and address of our own
local created LPC_GPIO_TypeDef structure and pass that to your test subject
peripheral object.

The actual port register address has bene passed into the GPIO object this time.
This is beneficial because all we need to do is pass and address of our own
local created LPC_GPIO_TypeDef structure and pass that to your test subject
peripheral object.

``` c++
TEST_CASE("Testing Gpio")
{
  // Initialized local LPC_GPIO_TypeDef objects with 0 to observe how the
  // Gpio class manipulates the data in the registers
  LPC_GPIO_TypeDef local_gpio_port;
  memset(&local_gpio_port, 0, sizeof(local_gpio_port));

  // Pins that are to be used in the unit test
  Gpio test_subject(&local_gpio_port, 0);

  // ... Rest of the test code
}
```

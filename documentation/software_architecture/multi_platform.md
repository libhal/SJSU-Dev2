# Multi-Platform Applications

At some point, you are going to build something that you may wish to port to
another microcontroller or system. SJSU-Dev2 is designed in a way that
makes doing this, without bloating your application's binary size with
implementation details from each platform you want to support.

See the example code `demos/multiplatform/dual_platforms/`.

The method for making a project multi-platform has 3 phases to it, see file for
more details.

## Phase 1: Create interface pointers
In this phase you will want to define all of the peripherals you plan to use as
pointers. Typically, references are preferred for interfaces, but references
cannot be re-assigned at runtime, thus we must use pointers.

``` C++
const sjsu::Gpio * status_led_gpio;
const sjsu::Gpio * power_on_gpio;
const sjsu::Gpio * kill_switch_gpio;

const sjsu::Spi * spi_bus;
const sjsu::I2c * i2c_bus;

const sjsu::Gpio * motor_direction_gpio;
const sjsu::Pwm * motor_controller_pwm;
```

## Phase 2: Assign pointers in `if constexpr` blocks per platform
Now that we have made our list of peripherals or application needs, we need to
assign these pointers to real peripheral driver objects for the specified
platform.

There will need to be `if constexpr` block for each platform you care to
support.

Within each block a `static` peripheral object will need to be created and
constructed for each of the pointers in phase 1. After construction, each
peripheral pointer will need to be assigned to the address of each `static`
peripheral object. In this phase you can also create temporary objects within
the block if this is found useful.

The `static` keyword is required for objects that have their addresses
referenced outside the scope of the `if constexpr` block. Using the `static`
keyword will allocate the object statically and will make the object available
for the whole lifetime of the application. Not doing this means that the objects
will be destructed after the program leaves the scope of the `if constexpr`
block. If this happens, the behavior of the program is undefined.

``` C++
if constexpr (sjsu::build::kPlatform == sjsu::build::Platform::lpc17xx)
{
  static sjsu::lpc17xx::Gpio lpc17xx_status_led_gpio(1, 9);
  static sjsu::lpc17xx::Gpio lpc17xx_poweron_gpio(1, 0);
  // etc...

  poweron_gpio    = &lpc17xx_poweron_gpio;
  status_led_gpio = &lpc17xx_status_led_gpio;
  // etc...
}
else if constexpr (sjsu::build::kPlatform == sjsu::build::Platform::lpc40xx)
{
  static sjsu::lpc17xx::Gpio lpc40xx_status_led_gpio(1, 9);
  static sjsu::lpc17xx::Gpio lpc40xx_poweron_gpio(1, 0);
  // etc...

  poweron_gpio    = &lpc40xx_poweron_gpio;
  status_led_gpio = &lpc40xx_status_led_gpio;
  // etc...
}
else if constexpr (sjsu::build::kPlatform == sjsu::build::Platform::linux)
{
  static sjsu::lpc17xx::Gpio linux_status_led_gpio(1, 9);
  static sjsu::lpc17xx::Gpio linux_poweron_gpio(1, 0);
  // etc...

  poweron_gpio    = &linux_poweron_gpio;
  status_led_gpio = &linux_status_led_gpio;
  // etc...
}
else
{
  sjsu::LogError("Invalid platform for this application!");
  sjsu::Halt();
}
```

## Phase 3: Pass references to devices and systems objects if any
If you are passing the L1 peripherals to devicesS then this phase is the time to
do it. This phase is outside of the `if constexpr` block and is where you list
and construct every hardware abstraction. Same goes for application level
objects. A small example below

``` C++
sjsu::Button poweron_button(*poweron_gpio);
sjsu::RCCarController rc_controller(*motor_direction_gpio,
                                    *motor_controller_pwm,
                                    /* etc ... */);
```

## Done
At this point you can begin using your peripherals, HALs, application layer.

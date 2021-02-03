# Finding Drivers and Libraries

The directory structure of libraries in SJSU-Dev2 is hierarchical where each
`layer` indicates a logical separation from the lowest layer, the CPU/Platform,
to the peripherals (systems in the periphery of CPU but not the CPU), to the
HAL, which signifies devices and systems external to the platform, and finally
to the application. The application layer tends to utilize a combination of
elements in the layers below. `utility/` and `newlib` are orthogonal to the
hierarchy thus they can be access by an layer of the code base.

## Tree showing code base structure

```
library/
├── platforms/
├── peripherals/
├── devices/
├── systems/
├── testing/
├── newlib/
├── third_party/
└── utility/
```

## utility

Contains system agnostic utility functions and classes for things like bit
manipulation, math functions, debugging libraries, logging, etc. Can be used by
any library layer of the code.

## platforms

Contains source files for initializing the microcontroller as well as
platform specific system definitions. This layer typically includes a
`startup.cpp`, which initializes the system then calls `main()`.
You shouldn't need to include anything from this layer.

```
platforms/
├── platform_0/
├── platform_1/
├── ...
├── platform_N/
└── common_l0_methods.hpp
```

## peripherals

Contains drivers for controlling peripherals internal to a microcontroller,
like GPIO, SPI, UART, I2C, ADC, PWM, etc...

```
peripherals/
├── platform_0/
├── platform_1/
├── ...
├── platform_N/
├── interface_1.hpp
├── interface_2.hpp
├── ...
└── interface_N.hpp
```

The folders of L1 are named after the platform. For example
`peripherals/lpc40xx` contains device drivers for the `lpc40xx` series of
MCUs. `peripherals/lpc40xx/gpio.hpp` would be the driver for the `lpc40xx`
gpio peripheral.

The files in L1, not within a platform folder are the peripheral interfaces. For
example, `peripherals/gpio.hpp` contains the `sjsu::Gpio` interface as well
as some supporting libraries. `sjsu::Gpio` will contain the class methods that
all drivers MUST implement. So you can be assured that calling
`sjsu::Gpio::SetHigh()` behaves in a consistent manor on all platforms.

## devices

HAL stands for Hardware Abstraction Layer, and contains drivers for
controlling devices and systems external to the MCU. These can be
sensors, displays, switches, memory, etc. devices device drivers will almost
always require L1 peripherals in order to operate. For example, an LCD driver
will need a set of Gpios in order to operate.

```
devices/
├── actuators/
├── audio/
├── boards/
├── communication/
├── memory/
├── power/
├── sensors/
├── switches/
└── etc.../
```

The format of the folder is similar to L1, but rather than having directories
for each platform, they are separated into categories.

## systems

Holds high layer software systems like graphics engines, task objects,
task schedulers, and command line interfaces and command line objects.

```
systems/
├── high_layer_module_0.hpp
├── high_layer_module_1.hpp
├── ...
├── high_layer_module_N.hpp
└── high_layer_modules_directory/
```

## testing

Holds high layer software systems like graphics engines, task objects,
task schedulers, and command lines. These should only be included for unit
testing purposes.

```
testing/
├── factory_test.hpp
├── freertos_mocks.cpp
├── main_test.cpp
└── testing_frameworks.hpp
```

## third_party

Contains third party projects that are used by the SJSU-Dev2 libraries.

```
third_party/
├── doctest
├── fakeit
├── fatfs
├── fff
├── font8x8
├── FreeRTOS
├── microrl
├── printf
└── third_party.mk
```

# Library/ Directory

Contains the core SJSU-Dev2 library source files. The library directory layout
is a hierarchical layout where each `layer` or `level` uses can only access
files form below its level. `utility/` and `newlib` are orthogonal to the hierarchy
thus they can be access by an level of the code base.

## Tree showing code base structure

```
library/
├── L0_Platform/
│   ├── platform_0/
│   ├── platform_1/
│   ├── ...
│   ├── platform_N/
│   └── common_l0_methods.hpp
├── L1_Peripheral/
│   ├── platform_0/
│   ├── platform_1/
│   ├── ...
│   ├── platform_N/
│   ├── interface_1.hpp
│   ├── interface_2.hpp
│   ├── ...
│   └── interface_N.hpp
├── L2_HAL/
│   ├── actuators/
│   ├── audio/
│   ├── boards/
│   ├── communication/
│   ├── memory/
│   ├── power/
│   ├── sensors/
│   ├── switches/
│   └── etc.../
├── L3_Application/
│   ├── high_level_module_0.hpp
│   ├── high_level_module_1.hpp
│   ├── ...
│   ├── high_level_module_N.hpp
│   └── high_level_modules_directory/
├── L4_Testing/
│   ├── factory_test.hpp
│   ├── freertos_mocks.cpp
│   ├── main_test.cpp
│   └── testing_frameworks.hpp
├── newlib/
├── static_libraries/
│   ├── platform_0/
│   ├── ...
│   └── platform_N/
├── third_party/
│   ├── catch2
│   ├── etl
│   ├── fakeit
│   ├── fatfs
│   ├── fff
│   ├── font8x8
│   ├── FreeRTOS
│   ├── microrl
│   ├── printf
│   └── third_party.mk
└── utility/
```

## utility

Contains system agnostic utility functions and classes for things like bit
manipulation, printing stack traces, logging, utility macros etc. Can be used by
any library layer of the code.

## L0_Platform

Contains source files for initializing the microcontroller as well as
platform specific system definitions. This level typically includes a
`startup.cpp`, which initializes the system then calls `main()`. You shouldn't
need to include anything from this level.

## L1_Peripheral

Contains code for controlling peripherals internal to a microcontroller,
like GPIO, SPI, UART, I2C, ADC, PWM, etc...

The files in the first level of L1 are the peripheral interfaces.
The folders in the first level of L1 are named after the supported platforms.
Within each platform folder are the interface implementations for each

## L2_HAL

HAL stands for Hardware Abstraction Layer, and contains drivers for
controlling devices and systems external to the MCU. These can be
sensors, displays, switches, memory, etc. L2 HALs use L1 peripherals to achieve
their goal of abstracting the hardware.

## L3_Application

Holds high level software systems like graphics engines, task objects,
task schedulers, and command line interfaces and command line objects.

## L4_Testing

Holds high level software systems like graphics engines, task objects,
task schedulers, and command lines.

## third_party

Contains third party projects that are used by the SJSU-Dev2 libraries.

# Demos/ Directory

SJSU-Dev2 has a bunch of demos (mostly for the SJTwo board though), that you can
use to learn how to use a particular device, module or peripheral, act as a
reference to indicate how the module should be used, test out specific features
of the board, and they can be starting templates for your project.

## 1st Layer
The first set of folders you will see in the `demos/` directory are:

* `arm_cortex/`
* `sjtwo/`
* `sjone/`
* `multiplatform/`
* `platform_1/`
* `platform_2/`
* `...`
* `platform_n/`
* `cpu_1/`
* `cpu_2/`
* `...`
* `cpu_n/`

### SJTwo Demos

Within `sjtwo/` you will find demos for that board. Since it is the main
development board used by SJSU-Dev2 contributors, it has the most demos. This
is an example of a platform demo folder which only includes that work on this
specific board or chip. Do not expect these to work on any other platform.

SJTwo is a special platform folder in that, it contains reference demos. So look
into this folder if you need to understand how to control PWM for your STM32,
atmega, or RISC-V board.

### Multiplatform Demos

These demos can be run and executed across platforms. Feel free to flash such
projects onto any of your boards. Usually includes OS or utility modules
demonstrations.

### ARM Cortex Demos

The `arm_cortex/` demos are an example of a multiplatform demo that work across
systems that use arm cortex chips. Such demos will not work for AVR, RISC-V,
MIPS and other CPUS. These demos should be loadable to any support ARM cortex
based microcontroller.

# 2nd Layer

Within each platform/cpu folder are the actual demos. You can go into them,
build them and flash them to the appropriate device.

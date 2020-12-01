# Programming Devices

Programming a device, also known as flashing a device, is where one take the
binary code of a program and writes it into the storage space of the device.
This guide goes into details about how to use the SJSU-Dev2 build system `make`
in order to build and program devices.

## Usage

```bash
make [target] [PLATFORM=[linux|lpc40xx|stm32f10x|...]]
make [target] [PLATFORM=platform] [JTAG=[stlink|jlink|...]]
make [target] [PLATFORM=platform] OPTIMIZE=2
```

## Finding Demos to Build and Program

All of the demonstration projects can be found in the `demos/` directory.

### SJTwo Demos

Within the `demos/sjtwo/` you will find demos. Since it is the main
development board used by SJSU-Dev2 contributors, it has the most demos.

### Multiplatform Demos

These demos can be run and executed across platforms. Feel free to flash such
projects onto any of your boards. These typically do not attempt to manipulate
hardware.

### Blue Pill Demos

The blue pill demos can be found in the `demos/stm32f10x` directory as that is
the name of the series of microcontroller that is present on the blue pill.

### ARM Cortex Demos

The `arm_cortex/` demos are an example of a multiplatform demo that work across
systems that use arm cortex chips. Such demos will not work for RISC-V, MIPS and
other CPUS. These demos should be loadable to any support ARM cortex based
microcontroller.

### Others

Search around and explore this directory. The first layer is always the name of
the platform and the next layer is all of the demonstration projects.

## Step 1. Build an application

Choose a demo to try out and `cd` into the directory. If you aren't sure which
one to choose for this guide, simply go into the `projects/hello_world` project
for starters and branch out later.

We need to build the application before we can attempt to program it into the
device.

```bash
make application
```

### Step 1.2. Build an application for the stm32f10x microcontroller

If you are using a different platform than the SJTwo board you WILL need to
specify the PLATFORM name. In this example, we are building an application for
the stm32f10x series of MCUs.

```bash
make application PLATFORM=stm32f10x
```

#### Step 1.2.1 (Optional) Enabling Optimizations

You can also set the optimization level in order to change the performance of
the program. In this example we set the optimization level to "s" which produces
the smallest binary size. This is helpful as most MCUs do not have a lot of
storage space, and this flag can allow developers to squeeze more code/features
into a application/product than if it wasn't used.

```bash
make application PLATFORM=stm32f10x OPTIMIZE=s
```

## Step 2. Programming Devices

Programming a device is nearly the same command as build an application but
replace the "application" target name with "flash". Like so:

```bash
make flash
```

The above command only works for the LPC series of microcontrollers which are
used for the SJTwo and SJOne boards.

In general, you will want to program a device using a JTAG or SWD debugger
such as an `stlink` debugger. To flash using a JTAG device, you need to supply
the `JTAG` parameter in the command like so:

```bash
make flash JTAG=stlink
```

Other options for `JTAG` are available such as `JTAG=jlink` or `JTAG=buspirate`.
`make flash` supports all debug devices that OpenOCD supports. `stlink` is the
preferred choice for SJSU-Dev2 users as it is cheaper and more widely available
on Amazon, Ebay, AliExpress and other online stores.

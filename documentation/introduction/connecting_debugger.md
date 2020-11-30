# Connecting a JTAG/SWD Debugger to a Device

JTAG and SWD debuggers are the preferred way to interact with devices in
SJSU-Dev2, as they provide two main benefits.

1. Can be used to program/flash devices, which tends to be faster than serial
   programming.
2. Can be used to debug code on device line-by-line, which is a powerful tool in
   debugging the state and behavior of a program over print statements.

SJSU-Dev2 leverages the programs OpenOCD and GDB for the purposes above, but
this combination has 1 caveat, which is that GDB does not have a clean way to
accept STDIN from a keyboard.

## Prerequisite Knowledge

SWD stands for Serial Wire Debug and is the favored solution for SJSU-Dev2 due
to its lower pin count compared to JTAG. SWD uses two pins for debugging, the
`SWCLK`, clock reference pin, and the `SWDIO`, data input/output pin. Some
development boards will omit the `SW` prefix to save space and will simply show
`CLK` and `DIO`.

JTAG can also be used in SJSU-Dev2 but has far more pins such as the following
`TDI`, `TMS`, `TCK`, and `TDO`.

With both of these standards, you will need to connect a jumper wire between the
SWD pins on the debugger device and the development board.

## Step 0: Getting the Parts

This example uses the STM32F10x Blue Pill as an example but this guide can also
be used for any other development board so long as the debugging pins are
available as connections.

If you do not have the parts you can buy them from Amazon using these links:

- [STLink v2](https://www.amazon.com/HiLetgo-Emulator-Downloader-Programmer-STM32F103C8T6/dp/B07SQV6VLZ/)
- [STM32F10x Blue Pill Board](https://www.amazon.com/HiLetgo-STM32F103C8T6-Minimum-Development-Learning/dp/B07VKSVM21/)
- [Bundle of both](https://www.amazon.com/Aideepen-Programming-Emulator-Downloader-STM32F103C8T6/dp/B07WRL3DF8/)

!!! Note
    There are other types of debuggers out there like the Segger brand of
    debuggers. These will work with SJSU-Dev2 as well, but they tend to be more
    expensive.

## Step 1: Connecting the Debugger

A connection to ground (`GND`) must be made between the debugger and the
development board in order for the devices to communicate.

!!! Danger
    DOUBLE AND TRIPLE CHECK YOUR CONNECTIONS! Incorrect connects can result in
    breaking a board, debugger or possible your computer.

### Step 1.1: Connecting JTAG

Connect jumpers from the `GND`, `TDI`, `TMS`, `TCK`, and `TDO` pins on the JTAG
debugger to the headers on the development board of the same name.

### Step 1.2: Connecting SWD

Connect jumpers from `GND`, `SWDIO` and `SWDCLK` to the pins on the board. If
the board supports both `SWD` and `JTAG` like many arm cortex boards do, then
connect the pins in the following way:

- `SWDIO` --> `TMS`
- `SWDCLK` --> `TCK`

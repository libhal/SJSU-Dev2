# Device Debugging

## Step 1: Prerequisites

Make sure that the device is connected to a SWD or JTAG debugger before
proceeding. This will not work if the debugger is not connected to the device.

## Step 2: Running the debugger

The `make` target to start debugging is `make debug`. In order for this to work
you need to set the variables `JTAG` and `PLATFORM`.

For example if you are using the `lpc40xx` platform and the `stlink` branded
debugger your command would look something like this:

```bash
make debug JTAG=stlink PLATFORM=lpc40xx
```

Example for `lpc17xx` and `jlink`

```bash
make debug JTAG=jlink PLATFORM=lpc17xx
```

!!! Tip
    For advanced users that want to use a different debugger, all you need to do
    in order to use it is to use the name of the debugging interface that can be
    found in the `tools/openocd/scripts/interface/` such as the `buspirate` or
    `cmsis-dap`. The `JTAG` variable is literally the name of the file
    without the `.cfg` extension.

!!! Tip
    Successful output should look something like the following:

    ``` bash
    Info : clock speed 4000 kHz
    Info : JTAG tap: lpc40xx.cpu tap/device found: 0x4ba00477
           (mfg: 0x23b (ARM Ltd.), part: 0xba00, ver: 0x4)
    Info : lpc40xx.cpu: hardware has 6 breakpoints, 4 watchpoints
    ```

!!! Error
    If you see your terminal get spammed with this:

        ``` bash
        Error: JTAG-DP STICKY ERROR
        Error: Invalid ACK (7) in DAP response
        Error: JTAG-DP STICKY ERROR
        Error: Could not initialize the debug port
        ```

    Then its a good chance that one of your pins is not connected.

## Step 2: Using GDB

If you do not know how to use GDB here is a
[GDB Cheat Sheet](http://darkdust.net/files/GDB%20Cheat%20Sheet.pdf).

At this point the board has been halted. You should be able to add breakpoints
to add breakpoints at this point. A typical first breakpoint for a program is
to set a breakpoint on main.

``` bash
>>> break main
```

Next you will want to reset the program back to the start using the following
command.

``` bash
>>> monitor reset halt
```

To begin running through the program use the `continue` command.

```bash
>>> continue
```

At this point you should see the source code of your `main.cpp` show up. Now you
can step through your code and set breakpoints using `step`, `next`, `finish`
and `continue`, `break`, etc.

!!! Note
    On boards with a factory bootloader, when you start debugging, you will
    notice that you cannot see the source code lines in the gdb shell. This is
    because the bootloader instructions are not associated with any addresses in
    your code, thus you will not see source code. This is fine. Continue with
    the guide.

!!! Note
    Typically you would use the `run` command to start the code. When performing
    firmware testing, the `run` command is not needed as the code is already
    "running" on the remote microcontroller.

!!! Note
    You may also notice that GDB also looks a look nicer in SJSU-Dev2. Thats
    because we use the
    [gdb-dashboard](https://github.com/cyrus-and/gdb-dashboard) which is an
    awesome tool for doing gdb debugging in general.

### Using `print` and `set variable` commands

A very helpful command for GDB is the `print` command.

```bash
>>> print a + 123
```

The statement above takes any expression and will print its result. For example
one could do something like this:

```bash
>>> print sjsu::stm32f10x::TIM1->CCER
```

The above expression will print the CCER register value.

You can also use the `set variable` command to actually change those values.
For example, if you are within a loop you force the loop `i` iterator variable
to 5. You can also change register values as well.

```bash
>>> set variable i = 5
>>> set variable sjsu::stm32f10x::USART1->CR1 = 1
```

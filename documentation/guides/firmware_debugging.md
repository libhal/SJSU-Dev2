# Debugging with OpenOCD and GDB

## Step 0: Prerequisites

In order to debug an application running on a microcontroller you need, hardware
tools that allow you to inspect the inner workings of a device. These debugging
interfaces are called JTAG debugger or "in-circuit" debugger.

Some development boards have debugging hardware built in. Many others will have
pins or ports available for using an external debugger like a **STLink** or
**Segger Jlink**.

## Step 1: Connecting the Debugger
!!! Danger
    DOUBLE AND TRIPLE CHECK YOUR CONNECTIONS! Not doing this right could destroy
    board and debugger.

!!! Tip
    Skip this step if you board has a debugger built in.

### Step 1.1: Connecting JTAG
Connect jumpers from the `GND`, `TDI`, `TMS`, `TCK`, and `TDO` pins on
the **J-Link** to the board's JTAG headers.

### Step 1.2: Connecting SWD
Connect jumpers from `GND`, `SWDIO` and `SWDCLK` to the pins on the board. If
the board supports both `SWD` and `JTAG` like many arm cortex boards do, then
connect the pins in the following way:

* `GND` --> `GND`
* `SWDIO` --> `TMS`
* `SWDCLK` --> `TCK`

### Step 1.3: Using a standard connector
Some development boards may use an interface that only requires connecting a
ribbon cable that your development board also has an port for, in which case,
simply connect the boards together with the ribbon cable.

## Step 2: Running the debugger
In order for you to be able to debug your microcontroller you need to know the
name of the debugging device. Collaborators typically use the `jlink`  or
`stlink` debuggers for their work thus it is generally supported and tested
with.

The `make` target to start debugging is `make debug`. In order for this to work
you need to set the variables `DEBUG_DEVICE` and `PLATFORM`.

For example if you are using the `lpc40xx` platform and the `stlink` branded
debugger your command would look something like this:

```bash
# Automatically works for any stlink debugger
make debug DEBUG_DEVICE=stlink PLATFORM=lpc40xx
```

Example for `lpc17xx` and `jlink`

```bash
# Automatically works for any jlink debugger
make debug DEBUG_DEVICE=jlink PLATFORM=lpc17xx
```

!!! Tip
    For advanced users that want to use a different debugger, all you need to do
    in order to use it is to use the name of the debugging interface that can be
    found in the `tools/openocd/scripts/interface/` such as the `buspirate` or
    `cmsis-dap`. The `DEBUG_DEVICE` variable is literally the name of the file
    without the `.cfg` extension.

!!! Tip
    Successful output should look something like the following:

    ``` bash
    Info : clock speed 4000 kHz
    Info : JTAG tap: lpc40xx.cpu tap/device found: 0x4ba00477
           (mfg: 0x23b (ARM Ltd.), part: 0xba00, ver: 0x4)
    Info : lpc17xx.cpu: hardware has 6 breakpoints, 4 watchpoints
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
!!! Tip
    Also a handy cheat sheet, see this PDF:
    [gdb cheatsheet](http://darkdust.net/files/GDB%20Cheat%20Sheet.pdf>)

At this point the board has been halted and reset. You should be able to add
breakpoints to add breakpoints at this point.

!!! note
    On boards with a factory bootloader, when you start debugging, you will
    notice that you cannot see the source code lines in the gdb shell. This is
    because the bootloader address's are not associated with any addresses in
    your code, thus you will not see source code. Generally a good thing to do
    is to set a breakpoint at the **reset interrupt service routine** or at
    **main()** and run the GDB `continue` command like so:

    ``` bash
    >>> break main
    >>> continue
    ```

!!! Note
    Typically you would use the `run` command to start the code. When performing
    firmware testing, the `run` command is not needed as the code is already
    "running" on the remote microcontroller.

!!! Note
    You may also notice that GDB also looks a look nicer in SJSU-Dev2. Thats
    because we use the
    [gdb-dashboard](https://github.com/cyrus-and/gdb-dashboard) which is an
    awesome tool for doing gdb debugging in general.

At this point you should see the source code of your `main.cpp` show up.
Now you can step through your code and set breakpoints using `step`,
`next`, `finish` and `continue`, `break`, etc.

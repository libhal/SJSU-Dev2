# Debugging with OpenOCD and GDB

This tutorial will use **HelloWorld** as an example. But this will work
for any application you build.

## Prerequisites

The official supported JTAG probes for the SJOne and SJTwo board is the
SEGGER J-LINK mini EDU. Any other J-Link device will work with no
modifications to the `sjtwo.cfg` file. Otherwise, change the
interface/source to the appropriate adapter. If you want

## Step 0: Solder JTAG Headers to board

Do as the title says if you haven't already.

## Step 2: Connecting the J-Link

Connect jumpers from the `GND`, `TDI`, `TMS`, `TCK`, and `TDO` pins on
the **J-Link** to the board's JTAG headers.

!!! Danger
    DOUBLE AND TRIPLE CHECK YOUR CONNECTIONS! Not doing this right
    could destroy board and debugger.

## Step 3: Run OpenOCD

To start openocd, run `make openocd`.

!!! Tip
    Successful output should look something like the following:

    ``` bash
    Info : clock speed 4000 kHz
    Info : JTAG tap: lpc40xx.cpu tap/device found: 0x4ba00477
           (mfg: 0x23b (ARM Ltd.), part: 0xba00, ver: 0x4)
    Info : lpc17xx.cpu: hardware has 6 breakpoints, 4 watchpoints
    ```

!!! Error
    If you see the following message:

    ``` bash
    Error: JTAG-DP STICKY ERROR
    Info : DAP transaction stalled (WAIT) - slowing down
    Error: Timeout during WAIT recovery
    Error: Debug regions are unpowered, an unexpected reset might have
        happened
    ```

    Then the SJOne board is being held in a RESET state. To fix this, either
    by reset the board using the reset button or by deassert in Telemetry.

!!! Error
    If you see your terminal get spammed with this:

        ``` bash
        Error: JTAG-DP STICKY ERROR
        Error: Invalid ACK (7) in DAP response
        Error: JTAG-DP STICKY ERROR
        Error: Could not initialize the debug port
        ```

    Then its a good chance that one of your pins is not connected.

## Step 5: Run GDB

Open another terminal and run `make debug`

At this point the board has been halted. You should be able to add
breakpoints to the program. You shouldn't see any source because you are
in the factory bootloader.

Do the following in the gdb command line interface:

``` bash
>>> break main
>>> continue
```

!!! Note
    Typically you would use the `run` command to start the code. When performing
    firmware testing, the `run` command is not needed as the code is already
    running on the remote microcontroller.

At this point you should see the source code of your `main.cpp` show up.
Now you can step through your code and set breakpoints using `step`,
`next`, `finish` and `continue`, `break`, etc.

!!! Tip
    For a gdb cheat sheet, see this PDF:
    [gdb cheatsheet](http://darkdust.net/files/GDB%20Cheat%20Sheet.pdf>)

!!! Tip
    I highly recommend
    [GDB-Dashboard](https://github.com/cyrus-and/gdb-dashboard) which is a nifty
    configuration of gdb that add on to make your life using gdb much more
    enjoyable.
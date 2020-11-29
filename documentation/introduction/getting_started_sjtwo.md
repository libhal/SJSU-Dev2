# Getting Started with the SJTwo

If you are using the SJTwo board this guide will be the simplest guide to
getting it up and running. If you are not using an SJTwo board, skip to the next
guide in the sequence.

## Building and Loading the "Hello World" Application

### Step 0: Move into project

```bash
cd projects/hello_world/
```

### Step 1: Build project

```bash
make application
```

This will take all of the relevant source code files to your project and
generate a binary file that can be loaded onto your board. These files can be
found in the `build/application` folder with the project.

<script id="asciicast-314699" src="https://asciinema.org/a/314699.js" async>
</script>

!!! Tip
    use the `make` by itself to get additional information on the
    different features of the build script.

!!! Tip
    To build for a different board you can specify the platform like so:
    `make application PLATFORM=lpc17xx`.
    This example will build the project for the SJOne board or any other device
    with an lpc17xx microcontroller on it.

### Step 2: Flash board

Before you can flash your board you will need to connect the SJTwo board to your
computer using a USB Micro cable. Once connected, to load the binary file into
your board, run:

```bash
make flash
```

If you want to specify the device port you can try:

For Ubuntu:

```bash
make flash PORT=/dev/ttyUSB0
```

For Mac OSX:

```bash
make flash PORT=/dev/tty.SLAB_USBtoUART
```

!!! Note
    If `make flash` could not find your device, try running it again. If
    it continues to fail then it could a few things such as:

    1. Confirm that the USB micro cable is plugged into the computer and the
       SJTwo board.
    2. Make sure that the USB micro cable supports data communication as some
       "security" based USB micro cables are designed to only allow charging but
       do not have data lines.
    3. Make sure that the device is not connected to any computer programs such
       as Telemetry, PuTTY, or minicom. If you are uncertain of this,
       disconnecting and reconnecting the USB cable will force the programs to
       disconnect from the device.
    4. Make sure that the CP2102n driver is installed on your machine. Ubuntu
       has it preinstalled. For OSX a google search for
       "CP2102n driver for <insert OS>" will point you to download sites for the
       driver.

### Step 3: Interacting with device

To view the serial output messages from board and interact with it, go to the
following URL and following the setup instructions:
[Telemetry](https://sjsu-dev2.github.io/Telemetry/)

After falling the web app instructions:

1. Set the drop down menu for the baud rate `38400` (very important)
2. Use the drop down menu on the upper right hand side page to the serial port
   that was used for flashing. Typically this is `/dev/ttyUSB0` or `/dev/ttyS3`
   or `/dev/tty.SLAB_USBtoUART`.
3. Press the **Connect** button.

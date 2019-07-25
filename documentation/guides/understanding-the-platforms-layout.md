# Understanding The Library Directory Layout

To understand better whats available to you

## Folder: `projects`

Contains projects **applications** and library files.

The default applications are:

>   - **HelloWorld**: Template for simple applications.
>   - **Hyperload**: Bootloader that makes uploading binaries to the
>     board fast
>   - `projects/`: folder full of firmware examples.

## Example Projects

The examples folder contains a set of examples that can be flashed on to
the supported platforms of SJSU-Dev2.

### Folder: `projects/examples/<board>/`

Set of example projects that work for a specific board or
micrcontroller.

### Folder: `projects/examples/multiplatform/`

This folder contains the example projects that work across
microcontrollers. You should be able to program any board with them.

## Folder: `projects/<project>/source/`

The `source` folder holds the `main.cpp` file and other source files you
want to create or use for your project. You can structure your
application's layout how ever you like within the source folder.

## Folder: `projects/<project>/source/`

The `source` folder holds the `main.cpp` file and other source files you
want to create or use for your project. You can structure your
application's layout how ever you like within the source folder.

## Build Folder

The `build` folder contains all of the files generated in the process of
creating the firmware binary that you flash onto your board. The `build`
folder contains a folder for every `make`-able target.

### Folder: `<project>/build/application/`

This folder holds the executables, `.bin` and `.hex`, that can be loaded
into your board. This directory also contains, disassembly file `.lst`,
dependency tree file `.map`, and the Executable and Linkable Format
`.elf` file.

Most important is the `.bin` file which is used to program your board.

# Folder: `<project>/build/compiled/`

This folder holds the results of compiling .c and .cpp files, object
files, which are generated during the compilation stage of building the
executable. They are then all linked together to create an `.elf` file
afterwards. The last phase after creating an `.elf` is to converts the
`.elf` to a `.hex` file to be loaded into your board's internal storage
memory.

# Folder: `<project>/build/test/`

This folder holds compiled test object files as well as the `test.exe`
executable file. Running the `test.exe` file will run through all of the
library tests as well as any local test files in your project.

# Folder: `<project>/build/coverage/`

This folder holds the test coverage files. Opening the `coverage.html`
in your browser will show you how much of the library code has been
tested.

## Folder: `library/`

Contains the core SJSU-Dev2 source files for each microcontroller and
abstractions for using GPIO, I2C, UART, Interrupts, etc.

# Folder: `library/utilities`

Contains system agnostic utility functions and classes for things like
bit manipulation, printing stack traces, logging, useful macros and
more.

# Folder: `library/L0_Platform`

Contains source files for initializing the microcontroller as well as
platform specific system definitions. This level typically includes a
`startup.cpp`, which initializes the system then calls main, and a
`interrupt_vector_table.cpp` file, which defines the interrupt vector
table for the controller. You shouldn't need to include anything from
this level, but on occasions you mayneed to include the plat

# Folder: `library/L1_Peripheral`

Contains code for controlling peripherals internal to a microcontroller,
like GPIO, SPI, UART, I2C, ADC, PWM, etc...

# Folder: `library/L2_HAL`

HAL stands for Hardware Abstraction Layer, and contains drivers for
controlling devices and systems external to the MCU. These can be
sensors, displays, switches, memory, etc.

# Folder: `library/L3_Application`

Holds high level software systems like graphics engines, task objects,
task schedulers, and command line interfaces and command line objects.

# Folder: `library/L4_Testing`

Holds high level software systems like graphics engines, task objects,
task schedulers, and command lines.

# Folder: `library/third_party`

Contains third party projects that are used by the SJSU-Dev2 libraries.
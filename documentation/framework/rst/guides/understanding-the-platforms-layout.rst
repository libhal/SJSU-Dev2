Understanding The Library Directory Layout
===========================================
To understand better whats available to you

Folder: :code:`projects`
-------------------------
Contains projects **applications** and library files.

The default applications are:

  * **HelloWorld**: Template for simple applications.
  * **Hyperload**: Bootloader that makes uploading binaries to the board fast
  * :code:`projects/`: folder full of firmware examples.

Example Projects
-------------------------------------------
The examples folder contains a set of examples that can be flashed on to the
supported platforms of SJSU-Dev2.

Folder: :code:`projects/examples/<board>/`
+++++++++++++++++++++++++++++++++++++++++++
Set of example projects that work for a specific board or micrcontroller.

Folder: :code:`projects/examples/multiplatform/`
+++++++++++++++++++++++++++++++++++++++++++++++++
This folder contains the example projects that work across microcontrollers.
You should be able to program any board with them.

Folder: :code:`projects/<project>/source/`
-------------------------------------------
The :code:`source` folder holds the :code:`main.cpp` file and other source
files you want to create or use for your project. You can structure your
application's layout how ever you like within the source folder.

Folder: :code:`projects/<project>/source/`
-------------------------------------------
The :code:`source` folder holds the :code:`main.cpp` file and other source
files you want to create or use for your project. You can structure your
application's layout how ever you like within the source folder.

Build Folder
-------------
The :code:`build` folder contains all of the files generated in the process of creating the firmware binary that you flash onto your board. The :code:`build`
folder contains a folder for every :code:`make`-able target.

Folder: :code:`<project>/build/application/`
+++++++++++++++++++++++++++++++++++++++++++++
This folder holds the executables, :code:`.bin` and :code:`.hex`, that can be
loaded into your board. This directory also contains, disassembly file
:code:`.lst`, dependency tree file :code:`.map`, and the Executable and
Linkable Format :code:`.elf` file.

Most important is the :code:`.bin` file which is used to program your board.

Folder: :code:`<project>/build/compiled/`
++++++++++++++++++++++++++++++++++++++++++
This folder holds the results of compiling .c and .cpp files, object files,
which are generated during the compilation stage of building the executable.
They are then all linked together to create an :code:`.elf` file afterwards.
The last phase after creating an :code:`.elf` is to converts the :code:`.elf` to a :code:`.hex` file to be loaded into your board's internal storage memory.

Folder: :code:`<project>/build/test/`
++++++++++++++++++++++++++++++++++++++
This folder holds compiled test object files as well as the :code:`test.exe`
executable file. Running the :code:`test.exe` file will run through all of the
library tests as well as any local test files in your project.

Folder: :code:`<project>/build/coverage/`
++++++++++++++++++++++++++++++++++++++++++
This folder holds the test coverage files. Opening the :code:`coverage.html` in
your browser will show you how much of the library code has been tested.

Folder: :code:`library/`
-------------------------
Contains the core SJSU-Dev2 source files for each microcontroller and
abstractions for using GPIO, I2C, UART, Interrupts, etc.

Folder: :code:`library/utilities`
++++++++++++++++++++++++++++++++++
Contains system agnostic utility functions and classes for things like bit manipulation, printing stack traces, logging, useful macros and more.

Folder: :code:`library/L0_Platform`
++++++++++++++++++++++++++++++++++++
Contains source files for initializing the microcontroller as well as platform
specific system definitions. This level typically includes a
:code:`startup.cpp`, which initializes the system then calls main, and a
:code:`interrupt_vector_table.cpp` file, which defines the interrupt vector
table for the controller. You shouldn't need to include anything from this
level, but on occasions you mayneed to include the plat

Folder: :code:`library/L1_Peripheral`
++++++++++++++++++++++++++++++++++++++
Contains code for controlling peripherals internal to a microcontroller, like GPIO, SPI, UART, I2C, ADC, PWM, etc...

Folder: :code:`library/L2_HAL`
+++++++++++++++++++++++++++++++
HAL stands for Hardware Abstraction Layer, and contains drivers for controlling
devices and systems external to the MCU. These can be sensors, displays,
switches, memory, etc.

Folder: :code:`library/L3_Application`
+++++++++++++++++++++++++++++++++++++++
Holds high level software systems like graphics engines, task objects, task
schedulers, and command line interfaces and command line objects.

Folder: :code:`library/L4_Testing`
+++++++++++++++++++++++++++++++++++
Holds high level software systems like graphics engines, task objects, task
schedulers, and command lines.

Folder: :code:`library/third_party`
++++++++++++++++++++++++++++++++++++
Contains third party projects that are used by the SJSU-Dev2 libraries.

Understanding The Library Directory Layout
===========================================
To understand better whats available to you

Folder: :code:`firmware`
-------------------------
Contains firmware **applications** and library files.

The default applications are:

  * **HelloWorld**: Template for simple applications.
  * **Hyperload**: Bootloader that makes uploading binaries to the board fast

Folder: :code:`firmware/<project>/source/`
-------------------------------------------
The :code:`source` folder holds the :code:`main.cpp` file and other source
files you want to create or use for your project. You can structure your
application's layout how ever you like within the source folder.

Folder: :code:`<project>/build/application/`
---------------------------------------------
This folder holds the executables, :code:`.bin` and :code:`.hex`, that can be
loaded into the SJTwo board. This directory also contains, disassembly file
:code:`.lst`, dependency tree file :code:`.map`, and the Executable and
Linkable Format :code:`.elf` file.

Folder: :code:`<project>/build/compiled/`
------------------------------------------
This folder holds object files generated during the compilation stage of
building the executable. They are then all linked together to create an
:code:`.elf` file afterwards. The last phase after creating an :code:`.elf` is
to converts the :code:`.elf` to a :code:`.hex` file to be loaded into SJTwo's
flash memory.

Folder: :code:`<project>/build/test/`
--------------------------------------
This folder holds compiled test object files as well as the :code:`test.exe`
executable file. Running the :code:`test.exe` file will run through all of the
library tests as well as any local test files in your project.

Folder: :code:`<project>/build/coverage/`
------------------------------------------
This folder holds the test coverage files. Opening the :code:`coverage.html` in
your browser will show you how much of the library code has been tested.

Folder: :code:`firmware/library/`
----------------------------------
Contains the core SJSU-Dev2 firmware source files, such as abstractions
for using GPIO, I2C, UART, Interrupts, etc.

Folder: :code:`firmware/library/utilities`
------------------------------------------
Contains system agnostic utility functions and classes for things like bit manipulation, printing stack traces, logging, helpful macros and more.

Folder: :code:`firmware/library/L0_LowLevel`
---------------------------------------------
Contains source files for initializing and the
processor/microcontroller. The startup.cpp file here is what calls main from a
:code:`ResetISR` is called.

Folder: :code:`firmware/library/L1_Drivers`
--------------------------------------------
Contains drivers for controlling peripheral internal to the microcontroller.

Folder: :code:`firmware/library/L2_HAL`
----------------------------------------
HAL stands for Hardware Abstraction Layer, and contains drivers for controlling
devices and systems external to the MCU. These can be sensors, displays,
switches, memory, etc.

Folder: :code:`firmware/library/L3_Application`
------------------------------------------------
Holds high level software systems like graphics engines, task objects, task
schedulers, and command lines.

Folder: :code:`firmware/library/L4_Testing`
--------------------------------------------
Holds high level software systems like graphics engines,
task objects, task schedulers, and command lines.

Folder: :code:`firmware/library/third_party`
---------------------------------------------
Contains third party projects that are used by the SJSU-Dev2 framework.

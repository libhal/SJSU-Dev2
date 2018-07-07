Understanding The Framework Layout
====================================

File Hierarchy
----------------

.. code-block:: bash

	firmware/
	├── HelloWorld
	│   ├── build
	│   │   ├── binaries
	│   │   ├── can-dbc
	│   │   ├── compiled
	│   │   ├── coverage
	│   │   └── test
	│   ├── env.sh -> /home/khalil/Git/SJSU-Dev2/env.sh
	│   ├── makefile -> /home/khalil/Git/SJSU-Dev2/makefile
	│   ├── project_config.hpp
	│   └── source
	│       └── main.cpp
	└── library
		├── config.hpp
		├── L0_LowLevel
		│   ├── cmsis_gcc.h
		│   ├── core_cm4.h
		│   ├── core_cmFunc.h
		│   ├── core_cmInstr.h
		│   ├── core_cmSimd.h
		│   ├── LPC40xx.h
		│   ├── startup.cpp
		│   ├── system_LPC407x_8x_177x_8x.h
		│   ├── uart0.min.cpp
		│   └── uart0.min.hpp
		├── L1_Drivers
		├── L2_Utilities
		├── L3_HAL
		├── L4_Application
		├── L5_Testing
		│   ├── main_test.cpp
		│   └── testing_frameworks.hpp
		├── LPC4078.ld
		├── newlib
		│   ├── newlib.cpp
		│   └── printf.cpp
		├── readme.md
		└── third_party
			├── catch2
			├── fakeit
			└── fff


Folder: :code:`firmware`
--------------------------
This folder is meant to hold the firmware **applications** you make.
The default applications are:

	* **HelloWorld**: Template for simple single-threaded applications.

Folder: :code:`firmware/<application>/source/`
-----------------------------------------------
The :code:`source` folder holds the :code:`main.cpp` file and other source
files you want to create or use for your project.

Folder: :code:`<applications>/build/binaries/`
-----------------------------------------------
This folder holds the executables (:code:`.hex`) that can be loaded into the
SJTwo board.

It also holds the disassembly file :code:`.lst`, linker file :code:`.map` and
the Executable and Linkable Format :code:`.elf` file.

Folder: :code:`<application>/build/_can_dbc/`
----------------------------------------------
The :code:`_can_dbc` folder holds the CAN message description files and
generated header files.

Folder: :code:`<application>/build/compiled/`
----------------------------------------------
This folder holds object files generated during the compilation stage of
building the executable. They are then all linked together to create an
:code:`.elf` file afterwards. The last phase after creating an :code:`.elf` is
to converts the :code:`.elf` to a :code:`.hex` file to be loaded into SJTwo's
flash memory.

Folder: :code:`<application>/build/test/`
-------------------------------------------
This folder holds compiled test object files as well as the :code:`test.exe`
executable file. Running the :code:`test.exe` file will run through all of the
library tests as well as any local test files in your project.

Folder: :code:`<application>/build/coverage/`
---------------------------------------------
This folder holds the test coverage files. Opening the :code:`coverage.html` in
your browser will show you how much of the library code has been tested.

Folder: :code:`firmware/library/`
----------------------------------
This folder holds the core SJSU-Dev2 firmware source files, such as abstractions
for using GPIO, I2C, UART, Interrupts, etc.

Folder: :code:`firmware/third_party`
-------------------------------------
This folder holds the third party projects that are used by the SJSU-Dev2
framework.

Folder: :code:`firmware/lib/L%d_%s`
------------------------------------
The folders that start with **L<some number>_<some folder name>** are kind of
self explanatory as to what they hold. For example, :code:`L0_LowLevel` holds
files that initialize the firmware before main is called and :code:`L1_Drivers`
are device drivers and so on and so forth.

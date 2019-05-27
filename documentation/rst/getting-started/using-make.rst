How to use make
=======================

make command syntax
--------------------

.. code-block:: bash

    make <TARGET> <VARIABLE#1>=<VALUE#1> <VARIABLE#2>=<VALUE#2> <VARIABLE#n>=<VALUE#n>

:code:`make` is the command that you need to type initially to run the make script. If you execute it alone at the top of the SJSU-Dev2 folder it will dump a help menu of information. Example:

.. code-block:: bash

    make

:code:`TARGET` is the thing you want build or an action you want to take. An
example targets that build stuff would be :code:`application`, :code:`test`,
and :code:`bootloader`. The application target would make an application binary,
bootloader will make a bootloader binary, and make test will compile and build
a test executable that you can run on your machine. Example:

.. code-block:: bash

    make application

:code:`VARIABLE`: name of a variable within the project that you can change to
modify how the target is built. The most used variable is the :code:`PLATFORM`
variable which you use to change which platform you are building the code for.

:code:`VALUE`: simply the value you want to put in the variable. The set of values you can put in the variable depends greatly on the actual variable. For example, the possible set of VALUES for VARIABLE :code:`PLATFORM` is the set of microcontrollers we support. Example:

.. code-block:: bash

    make application PLATFORM=lpc40xx

List of Targets
----------------

General Targets
++++++++++++++++
- **application**: Builds application program to be flashed onto board.
- **bootloader**: Builds bootloader program.
- **flash**: Write application to board.
- **burn**: Write bootloader to microcontroller's bootloader section.
- **clean**: Removes :code:`build/` folde which holds all of the projects
  current build files. Useful to force SJSU-Dev2 to build all files over again.
- **cleaninstall**: Removes :code:`build/` folder then runs
  :code:`make application`, then runs :code:`make flash`
- **telemetry**: Will open up the Telemetry chrome web application, which can be
  used to interact with your board.
- **help**: shows a help menu similiar to what you see here.

System Debug Commands
++++++++++++++++++++++
- **openocd**: Open up openocd (Open On-Chip Debugger) (only works for LPC40xx
  currently)
- **debug**: Opens gdb for your specific platform (only works for arm
  processors)
- **multi**-debug: Opens multiarchitecture gdb

SJSU-Dev2 Collaberation Commands:
++++++++++++++++++++++++++++++++++
- **lint**: Checks that the syntax and code style match the requirements for
  SJSU-Dev2.
- **tidy**: Checks that the variable names fit the SJSU-Dev2 coding style.
- **presubmit**: run presubmit checks that the continous integration system runs
             before allowing a pull request's code to be allowed into the code
             base. See `presubmit checks`_

.. _presubmit checks: ../contributing/presubmit-checks.html


Makefile Diagnostic Targets
++++++++++++++++++++++++++++
- **show-lists**: dumps the variables of the makefile to stdout.

List of Variables
------------------
DEVICE
+++++++
Used by "make flash" to specify a direct path to a serial port connected to a
device running the Hyperload bootloader

    make flash DEVICE="/dev/ttyUSB0"

OPT
++++
Set the optimization level of the compiler. Default is optimization level 0.
Available optimization levels for GCC are:
    0: Low to no optimization. Only trivial and quick optimizations will be
       considered
    1: Level 1 optimization. Optimizes the executable, compilation time will
       increase.
    2: Level 2 optimization. Optimizes the executable further. Performs all
       optimizations that do not sacrafice memory to increase runtime
       performance.
    3: Highest level of optimization. Typically increases binary size
       significatly.
    s: Optimize for size. Will perform all optimizations that reduce the size
       of the binary.

TEST_ARGS
++++++++++
Set of tests you would like to run. Text must be surrounded by [] and be a set
comma deliminated.

  Example of running only i2c and adc tests with the -s flag to show
  successful assertions:

        make run-test TEST_ARGS="-s [i2c,adc]"

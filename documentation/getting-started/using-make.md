# How to use make

## Make Command Syntax

!!! note ""  
    make < TARGET> <VARIABLE#1>=<VALUE#1> <VARIABLE#2>=<VALUE#2> <VARIABLE#n >=<VALUE#n >


`make` is the command that you need to type initially to run the make
script. If you execute it alone at the top of the SJSU-Dev2 folder it
will dump a help menu of information. Example:


!!! note ""
    make


`TARGET` is the thing you want build or an action you want to take. An
example targets that build stuff would be `application`, `test`, and
`bootloader`. The application target would make an application binary,
bootloader will make a bootloader binary, and make test will compile and
build a test executable that you can run on your machine. Example:

!!! note ""
    make application


`VARIABLE`: name of a variable within the project that you can change to
modify how the target is built. The most used variable is the `PLATFORM`
variable which you use to change which platform you are building the
code for.

`VALUE`: simply the value you want to put in the variable. The set of
values you can put in the variable depends greatly on the actual
variable. For example, the possible set of VALUES for VARIABLE
`PLATFORM` is the set of microcontrollers we support. Example:

!!! note ""
    make application PLATFORM=lpc40xx


## List of Targets

### General Targets

  - **application**: Builds application program to be flashed onto
    board.
  - **bootloader**: Builds bootloader program.
  - **flash**: Write application to board.
  - **burn**: Write bootloader to microcontroller's bootloader section.
  - **clean**: Removes `build/` folde which holds all of the projects
    current build files. Useful to force SJSU-Dev2 to build all files
    over again.
  - **cleaninstall**: Removes `build/` folder then runs `make
    application`, then runs `make flash`
  - **telemetry**: Will open up the Telemetry chrome web application,
    which can be used to interact with your board.
  - **help**: shows a help menu similiar to what you see here.

### System Debug Commands

  - **openocd**: Open up openocd (Open On-Chip Debugger) (only works for
    LPC40xx currently)
  - **debug**: Opens gdb for your specific platform (only works for arm
    processors)
  - **multi**-debug: Opens multiarchitecture gdb

### SJSU-Dev2 Collaberation Commands:

  - **lint**: Checks that the syntax and code style match the
    requirements for SJSU-Dev2.

  - **tidy**: Checks that the variable names fit the SJSU-Dev2 coding
    style.

  -   - **presubmit**: run presubmit checks that the continous
        integration system runs  
        before allowing a pull request's code to be allowed into the
        code base. See [presubmit
        checks](../contributing/presubmit-checks.html)

### Makefile Diagnostic Targets

  - **show-lists**:
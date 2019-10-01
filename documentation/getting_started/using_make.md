# How to use make

## Make Command Syntax

    make <TARGET> <VARIABLE_1>=<VALUE_1> <VARIABLE_n>=<VALUE_n>

`make` is the command that you need to type initially to run the make
script. If you execute it alone at the top of the SJSU-Dev2 folder it
will dump a help menu of information. Example:

    make

### Target

`TARGET` is the thing you want build or an action you want to take. An
example targets that build stuff would be `application`, `test`, and `flash`.
The `application` target would make an application binary, `make test` will
build unit test code and run the test executable on your machine.

And example of making an application:

    make application

### Variables

`VARIABLE`: name of a variable within the project that you can change to
modify how the target is built. The most used variable is the `PLATFORM`
variable which you use to change which platform you are building the
code for.

`VALUE`: simply the value you want to put in the variable. The set of
values you can put in the variable depends greatly on the actual
variable. For example, the possible set of VALUES for VARIABLE
`PLATFORM` is the set of microcontrollers we support. Example:

    make application PLATFORM=lpc40xx

## List of Targets

### General Targets

- **application**: Builds application program to be flashed onto board.
- **bootloader**: Builds bootloader program.
- **flash**: Write application to board.
- **jtag-flash**: Write bootloader to microcontroller's bootloader section.

### Cleaning Targets
- **clean**: Removes `build/` folder which holds all of the projects current
  build files. Cleaning is useful to force SJSU-Dev2 to build all files over
  again.
- **library-clean**: Removes `library/static-libraries/<platform>/` folder which
  holds all of the archives libraries generated from the library folder. It is
  important to know that this only removes the the static_libraries from the
  selected platform.
- **purge**: deletes `build/` and the whole `library/static-libraries/`
  directory
- **cleaninstall**: Removes `build/`, runs `make application`, then runs
  `make flash`

### Debug Commands

- **debug**: connects to a microcontroller using OpenOCD and creates a GDB
  session.
- **test-debug**: Debug test executable in GDB (currently uses system's GDB not
  llvm)

### Collaberation Commands

- **lint**: Checks that the syntax and code style match the
  requirements for SJSU-Dev2.
- **tidy**: Checks that the variable names fit the SJSU-Dev2 coding
  style.
- **presubmit**: run presubmit checks that the continous integration system runs
  before allowing a pull request's code to be allowed into the code base.
  See [presubmit checks](../contributing/presubmit-checks.md)

### Makefile Diagnostic Targets

- **show-lists**: dumps variable contents of the makefile. Useful when making
changes to the make/build system.

### Additional Targets
- **telemetry**: Will open up the Telemetry chrome web application, which can be
  used to interact with your board.
- **help**: shows a help menu similiar to what you see here.
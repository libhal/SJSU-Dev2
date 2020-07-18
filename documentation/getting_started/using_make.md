# How to use make

## Make Command Syntax

    make <TARGET> <VARIABLE_1>=<VALUE_1> ... <VARIABLE_n>=<VALUE_n>

`make` is the command that you need to type initially to run the make script. If
you execute it alone in a project it will present to you a help menu. Example:

    make

### Target

`TARGET` is an action you want to take or something you want build. A few
examples of targets you can use with SJSU-Dev2 are `application`, `test`, and
`flash`.

The `application` target would make an application binary,
`make test` will build unit test code and run the test executable on your
machine.

An example of making an application:

    make application

### Variables

`VARIABLE`: name of a variable within the project that you can change to modify
how the target is built. The most used variable is the `PLATFORM` variable which
you use to change which platform you are building the code for.

`VALUE`: the value you want to put in the variable. The set of values you can
put in the variable depends greatly on the actual variable. For example, the
possible set of VALUES for VARIABLE `PLATFORM` is the set of microcontrollers we
support. Example:

    make application PLATFORM=lpc40xx

## Usage

    make [target] [PLATFORM=[linux|lpc40xx|lpc17xx|...]]
    make [target] [PLATFORM=platform] [JTAG=[stlink|jlink|...]]
    make [target] [PLATFORM=platform] OPTIMIZE=2

## Typical Examples

Build an application

    make application

Build application for stm32f10x

    make application PLATFORM=stm32f10x

Set platform to stm32f10x and set the optimization level to "s" which produces
the smallest binary size.

    make application PLATFORM=stm32f10x OPTIMIZE=s

Debug stm32f10x platform application using an stlink SWD/JTAG debugger. For this
to succeed, the application binary must already be built.

    make debug PLATFORM=stm32f10x JTAG=stlink

Build test code. To demonstrate this, go into the
`projects/continuous_integration` folder and run the following command.

    make test

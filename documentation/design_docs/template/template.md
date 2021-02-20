# Design Document Template

- [Design Document Template](#design-document-template)
- [Location](#location)
- [Type](#type)
- [Background](#background)
- [Overview (all)](#overview-all)
- [Detailed Design (all)](#detailed-design-all)
  - [Build CLI (Build)](#build-cli-build)
  - [API (Interfaces/Implementation/Structure/Software Architecture)](#api-interfacesimplementationstructuresoftware-architecture)
  - [Platform Porting (platform)](#platform-porting-platform)
    - [Definitions File](#definitions-file)
    - [Linker Script](#linker-script)
    - [Host Communication: Stdout & stdin](#host-communication-stdout--stdin)
    - [Millisecond Counter](#millisecond-counter)
    - [Heap Allocation](#heap-allocation)
    - [FreeRTOS](#freertos)
    - [Interrupt Vector Table](#interrupt-vector-table)
    - [Writing .mk file](#writing-mk-file)
    - [Startup](#startup)
- [Caveats](#caveats)
- [Future Advancements (optional)](#future-advancements-optional)
- [Testing Plan](#testing-plan)
  - [Unit Testing Scheme (only for interface/system architecture)](#unit-testing-scheme-only-for-interfacesystem-architecture)
  - [Integration Testing (only for build)](#integration-testing-only-for-build)
  - [Demonstration Project (only for implementation/system architecture)](#demonstration-project-only-for-implementationsystem-architecture)

# Location
Specify which library folder this module belongs to. For example:
`peripherals`.

# Type
This should be 1 line and can be any of the following:

* Interface
    * When designing a class interface that can be implemented in many ways,
    then write this.
* Structure
    * When designing a complex data structure then write this.
* Implementation
    * When implementing an interface or a class without an interface write this.
* Software Architecture
    * When implementing something that usually involves a system of software
      modules write this.
* Platform
    * When adding or changing a platform write this.
* Build
    * When adding or changing an aspect of the build system write this.

# Background
Add a section explaining the background of the part. For example if you were
making a module for an LCD screen, you can talk about the communication protocol
used to communicate with it, its size, its use cases, and any other detailed
information about how this system works.

# Overview (all)
A short overview of:

1. What you are designing.
2. How it adds to code base.
3. Some examples of how this can be used, if applicable.

# Detailed Design (all)

## Build CLI (Build)
Write out how a user will be able to utilize the new build feature.

```bash
make <TARGET> <NEW_PARAMETER>=[option1/option2/option3/...]
```

## API (Interfaces/Implementation/Structure/Software Architecture)
Write the class/structure definition of the module. Do not include comments
here. Make this nice and simple. The names of methods and parameters should be
obvious to those experienced in the field.

```C++
class ApiTemplate
{
 public:
  virtual void Initialize() = 0;
  virtual void Method1() = 0;
  virtual Status Method2(int param1, Type param_type) = 0;
  virtual void Method3() = 0;
  // ...
};
```

## Platform Porting (platform)

### Definitions File
Provide link to the .h header file full of definitions.

### Linker Script
Write out the linker script here or explain which linker script you will build
from. If you are deriving from another linker script, explain what things you
are changing.

### Host Communication: Stdout & stdin
Explain how you are going to handle stdout/stdin. Will you use UART, USB, RS232,
etc?

### Millisecond Counter
Provide a way to get a millisecond counter used for `Uptime()`.

### Heap Allocation
Provide the way you are handling heap allocation on this platform.

### FreeRTOS
Provide how you are integrating FreeRTOS. This is typically just listing which
FreeRTOS port files you are going to use for the platform.

### Interrupt Vector Table
Explain how you are integrating this interrupt vector table into the port.

### Writing .mk file
Write a rough draft of the submake file for the platform.

### Startup
Finally, explain how you are going to put all of the above together to make a
proper startup/reset procedure, prior to calling `main()`.

# Caveats
Explain any caveats about your design. This area is very important to make it
easy for those reading this design document to understand why they should or
should not use a particular design.
This area is explicitly here for providing known costs inherit in the design,
such as:

* Performance costs
    * Does this solution have any inheriant performance issues.
    * Example: Gpio Interrupt counter works well when used for something that is
      low frequency. If too many signals come in, or the pin is floating, then
      many interrupts willbe fired, leaving very little time for the processor
      to do work.
* Space costs
    * Does your module use up a lot of memory.
    * Example: SSD1308 OLED display driver makes a (128x64)/8 block of memory
* Compile time costs
    * Does this feature, when used, result in slower compile times.
    * Example: Use of this module utilizes complex template meta-programming and
      this results in an additional 2s of build time when compiling hello_world
      project.
* Usage costs
    * How difficult, complicated, or inelegant is using this module.
    * Example: Usage of this module requires coordination between 5 seperate
      modules, for which, if just 1 of them is not initialized correctly, will
      result in undefined behavior.
* Safety costs
    * Something about the feature that may invoke undefined behavior
    * Example: If the implementation's refresh method has not been called within
      5 seconds of each other, the behavior of the module will be undefined.
* Portability costs
    * Is this solution specific to a particular compiler
    * Example: The fixed sized sjsu::List<> object utilizes hidden std library
      structure definitions depending on the specific stl library or compiler,
      so using this on anything other than GCC or CLANG, it will fallback to
      another type, but that may not properly reflect the amount of memory being
      used.

# Future Advancements (optional)
Add a list of improvements that can improve or extend this further in the
future, state them here. Otherwise, put "N/A".

# Testing Plan
This section is irrelevant to **Interface** and **Structure** type documents.

## Unit Testing Scheme (only for interface/system architecture)
Please explain what techniques you plan on using to test the

## Integration Testing (only for build)
Write a plan for testing that the new build feature works across platforms.
Using TravisCI and checking manually that it completed as expected is the usual
approach.

## Demonstration Project (only for implementation/system architecture)
Write up simplified code example that demonstrates how your system will be used.
This should not be as detailed as your actual demonstration project. Should
be a small snippet that shows the simplest usage of your module.

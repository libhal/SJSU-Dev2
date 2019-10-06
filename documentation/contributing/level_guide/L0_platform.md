# Level 0 Platform Guidelines

Level 0 is the level for specific platforms. Working in this level means
you are working on a specific platforms low level code or that you are
add a new platform to the environment. Making a new platform is as
simple as adding a new folder in the platform with the exact name of the
platform you want to support (example: lpc17xx or atmega328p), and
adding and refactoring vendor code for the interrupt vector table
startup code.

The following steps goes over how to add each type vendor file to
SJSU-Dev2.

## Memory map definition .h files

Typically these are written in C and use macro judiciously. For
SJSU-Dev2 we need to put those definitions within the platform's
namespace as follows:

``` c++
// Sample name for this file could be "LPC40xx.hpp"
#pragma once

namespace sjsu
{
namespace lpc40xx
{
#include "LPC40xx.h"
} // lpc40xx
} // sjsu
```

The original header file must NOT be included in any library in SJSU-Dev2 to
prevent the global namespace from being contaminated.

## Linker Script

All platforms will require a linker script to describe to format of the
binary to the GCC linker. Typically vendor linker scripts do not need to
modified when brought into SJSU-Dev2. But the name of the linker script
must become `application.ld` and/or `bootloader.ld`. `application.ld`
will be used when the user uses `make application` and `bootloader.ld`
will be used when users use `make bootloader`.

### Definitions required for linker script

#### Define `.data` and `.bss` section table in the following way

```
section_table_start = .;
data_section_table = .;
LONG(LOADADDR(.data));
LONG(    ADDR(.data));
LONG(  SIZEOF(.data));
# Add more if needed
# LONG(LOADADDR(.data_N));
# LONG(    ADDR(.data_N));
# LONG(  SIZEOF(.data_N));
data_section_table_end = .;
bss_section_table = .;
LONG(    ADDR(.bss));
LONG(  SIZEOF(.bss));
# Add more if .bss sections if needed
# LONG(    ADDR(.bss_N));
# LONG(  SIZEOF(.bss_N));
bss_section_table_end = .;
section_table_end = . ;
```

#### Provide a symbol for `heap_start` and `heap_end`.

In order for `malloc` and new to work, you need to define the `heap_start`
and `heap_end` in your linker script. Below is an example of how to
define start ane ends of the heap.

```
# Put this near the end of the linker script.
# the . symbol is used to denote the current position of memory pointer
PROVIDE(heap_start =  .);
# __RAM_TOP is a macro that specifies where the end of RAM is. May be
# something different for the MCU you are porting.
PROVIDE(heap_end = __RAM_TOP);
```

The best option is to choose a defined region of ram just for heap such
that the stack pointer and heap pointer never cross each other. The
LPC40xx boards, has 96kB ram with 64kB used for RAM1 and 32kB for RAM2.
RAM1 and RAM2 are not contiguous blocks of ram in LPC40xx, so RAM1 is
used for `.data`, `.bss` and stack, where RAM2 is defined just for heap
memory.

## Startup

Typically the vendor provides a `startup.c` or `startup.cpp` file for
your use. Integration of this startup file must be converted to C++ and
follow the coding standards of SJSU-Dev2. This usually requires a bit of
rewriting of the startup code to comply with SJSU-Dev2 coding standards.

In order for a board to be fully compatible with SJSU-Dev2's
multi-platform examples the following things need to be setup before `int
main()` is called:

## [Porting Newlib](https://www.embecosm.com/appnotes/ean9/ean9-howto-newlib-1.0.html)

### Initialize RAM sections

- `.data` section must be copied into RAM from ROM. Failure to do
  so mean that initialized global variables will be undefined.
- `.bss` section must be initialized to zero. Failure to do so
  means undefined global and statically allocated variables will be
  undefined and will typically result in a hard fault when
  attempting to use stdlibc and stdlibc++.

### Host Communication

- There must be some way to communicate with a host computer. This
  is usually done via UART or USB-CDC and either method is
  acceptable, but UART tends to be the easiest.
- Update the STDOUT and STDIN callback functions by using
  `SetStdout` and `SetStdin`, found in the
  `library/newblib/newlib.h` Example:

``` c++
int Lpc40xxStdOut(const char * data, size_t length)
{
  uart0.Write(reinterpret_cast<const uint8_t *>(data), length);
  return length;
}

int Lpc40xxStdIn(char * data, size_t length)
{
  uart0.Read(reinterpret_cast<uint8_t *>(data), length);
  return length;
}
```

### malloc & new

Following Linker script part 2 for heap should handle this for you.

## Millisecond Timer:

- There needs to be some source of time keeping, preferably 64-bit. This could
  be a timer peripheral timer but it is preferred to use a timer that is a part
  of the CPU and not the MCU specifically. For example, each Arm Cortex chip has
  a system tick timer, thu we can reduce the amount of code necessary for each
  microcontroller that uses an Arm Cortex Mx (where x is any of them), by simply
  embedding a 64-bit counter within the SysTick ISR call and SysTick object.

- Once you have your time keeping source you will need to inject it into the
  timer system by running `SetUptimeFunction(Lpc40xxUptime);` where
  `Lpc40xxUptime()` is a function that returns the uptime count as a 64-bit
  value.

## FreeRTOS:

- Add the appropriate processor port.c and portmacro.h file to the processor
  platform in L0. See `library/L0_Platform/arm_cortex/m3/freertos/port.c`. Make
  sure you are using the GCC variant.
- Make sure that the appropriate interrupt service routines and timers have been
  setup. This variates depending on the architecure. On arm, there is the
  `vPortSVCHandler`, `xPortPendSVHandler`, and `vSysTickHandler`. Be sure to
  inject these using the interrupt peripheral class from within the
  `void vPortSetupTimerInterrupt(void)` function vs putting them directly into
  the interrupt vector table if possible. Doing this will reduce code size if
  FreeRTOS is not used.
- Because the standard for SJSU-Dev2 is to have deterministic memory
  requirements, in order to use FreeRTOS you need to statically allocate your
  idle task memory using the following block of code:

## Interrupt Vector Table

For organization purposes it is preferred that the interrupt vector table
(IVT) is placed in the L0 folder as its own file rather than putting it
in the `startup.cpp` file as many vendors do.

## Writing .mk file

``` Makefile
# Add library source files, typically its just the startup.cpp file
LIBRARY_LPC40XX += $(LIB_DIR)/L0_Platform/lpc40xx/startup.cpp
LIBRARY_LPC40XX += $(LIB_DIR)/L1_Peripheral/cortex/interrupt_vector_table.cpp

# Add test source files if applicable
TESTS += $(LIB_DIR)/L0_Platform/lpc40xx/startup_test.cpp

# Add any files or folders that should NOT be evaluated via the linter.
# System headers and register description files to be ignored.
LINT_FILTER += $(LIB_DIR)/L0_Platform/lpc40xx/LPC40xx.h
LINT_FILTER += $(LIB_DIR)/L0_Platform/lpc40xx/system_LPC407x_8x_177x_8x.h

# This call generates a static library for your platform
# $(eval $(call BUILD_LIRBARY, - calls the build library macros
# liblpc40xx - is the name of the library file created and to be linked in.
#              must be unique.
# LIBRARY_LPC40XX - is the variable holding the list of sources files to
#                   build in to the static library.
$(eval $(call BUILD_LIRBARY,liblpc40xx,LIBRARY_LPC40XX))

# (optional) include additional sub directory make files if applicable
# in this case, the LPC40xx uses an Arm Cortex-M4 so we want to add its
# system headers to our include path.
include $(LIB_DIR)/L0_Platform/arm_cortex/m4/m4.mk
```

## Testing L0 Platform

!!! Error
    Testing of IVT and and startup sequence has not been integrated into
    SJSU-Dev2.

## Optional things to port

### FatFS

Add a file `diskio.cpp` in the platform folder. Follow the code example
below:

!!! Warning
    This section is currently missing. Add a section for this after github
    issue #400_ is resolved which adds how to dependency inject a callback
    function FatFS.

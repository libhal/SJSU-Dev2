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

Typically these are written in C and use macro judiciously.
This is a problem as macro definitions cannot be namespaced and are very likely to create

For SJSU-Dev2 we need to put those definitions within the platform's
namespace as follows:

``` c++
#pragma once

namespace sjsu
{
namespace lpc40xx
{
// Put the rest of the header definitions here
} // lpc40xx
} // sjsu
```

Convert all `#define` to `constexpr` if possible and if not convert to `inline`
variables. See this commit to see how this was done for the
`platforms/lpc40xx/LPC40xx.h` register definitions file:

https://github.com/SJSU-Dev2/SJSU-Dev2/commit/40d89452260e043e0e1092525e5f3210b7cfe1ef#diff-1eae1a62a8a18ba66cfce475cf17db5a

## Linker Script

All platforms will require a linker script to describe to format of the
binary to the GCC linker. Typically vendor linker scripts do not require too much
modified when brought into SJSU-Dev2. But the name of the linker script
must be `linker.ld`.

### Definitions required for linker script

#### Define `.data` and `.bss` section table in the following way

```linker
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

```linker
# Put this near the end of the linker script.
# the . symbol is used to denote the current position of memory pointer
PROVIDE(heap_start =  .);
# __RAM_TOP is a macro that specifies where the end of RAM is. May be
# something different for the MCU you are porting.
PROVIDE(heap_end = __RAM_TOP);
```

If possible the best option is to choose a defined region of ram just for heap
such that the stack pointer and heap pointer never cross each other. The LPC40xx
boards, has 96kB ram with 64kB used for RAM1 and 32kB for RAM2. RAM1 and RAM2
are not contiguous blocks of ram in LPC40xx, so RAM1 is used for `.data`, `.bss`
and stack, where RAM2 is defined just for heap memory.

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
  `library/newlib/newlib.h` Example:

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

Instructions for LinkerScript part 2 for heap should handle this for you, so no
needed work on your side.

## Millisecond Timer

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
  platform in L0. See `library/platforms/arm_cortex/m3/freertos/port.c`. Make
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

For organization purposes it is preferred that the interrupt vector table (IVT)
is placed in the `startup.cpp` file as many vendors do. SJSU-Dev2 utilizes a
single interrupt vector that redirects to the assigned interrupt handler using.

Example using Cortex M3/M4/M7:

``` C++
// Platform interrupt controller for Arm Cortex microcontrollers.
sjsu::cortex::InterruptController<sjsu::lpc40xx::kNumberOfIrqs,
                                  __NVIC_PRIO_BITS>
    interrupt_controller;

SJ2_SECTION(".isr_vector")
const sjsu::InterruptVectorAddress kInterruptVectorTable[] = {
  // Core Level - CM4
  &StackTop,                           // 0, The initial stack pointer
  ArmResetHandler,                     // 1, The reset handler
  interrupt_controller.LookupHandler,  // 2, The NMI handler
  ArmHardFaultHandler,                 // 3, The hard fault handler
  interrupt_controller.LookupHandler,  // 4, The MPU fault handler
  interrupt_controller.LookupHandler,  // 5, The bus fault handler
  interrupt_controller.LookupHandler,  // 6, The usage fault handler
  nullptr,                             // 7, Reserved
  nullptr,                             // 8, Reserved
  nullptr,                             // 9, Reserved
  nullptr,                             // 10, Reserved
  vPortSVCHandler,                     // 11, SVCall handler
  interrupt_controller.LookupHandler,  // 12, Debug monitor handler
  nullptr,                             // 13, Reserved
  xPortPendSVHandler,                  // 14, FreeRTOS PendSV Handler
  interrupt_controller.LookupHandler,  // 15, The SysTick handler
  // Chip Level - LPC40xx
  interrupt_controller.LookupHandler,  // 16, 0x40 - WDT
  interrupt_controller.LookupHandler,  // 17, 0x44 - TIMER0
  interrupt_controller.LookupHandler,  // 18, 0x48 - TIMER1
  interrupt_controller.LookupHandler,  // 19, 0x4c - TIMER2
  interrupt_controller.LookupHandler,  // 20, 0x50 - TIMER3
  interrupt_controller.LookupHandler,  // 21, 0x54 - UART0
  interrupt_controller.LookupHandler,  // 22, 0x58 - UART1
  interrupt_controller.LookupHandler,  // 23, 0x5c - UART2
  interrupt_controller.LookupHandler,  // 24, 0x60 - UART3
  interrupt_controller.LookupHandler,  // 25, 0x64 - PWM1
  interrupt_controller.LookupHandler,  // 26, 0x68 - I2C0
  interrupt_controller.LookupHandler,  // 27, 0x6c - I2C1
  interrupt_controller.LookupHandler,  // 28, 0x70 - I2C2
  interrupt_controller.LookupHandler,  // 29, Not used
  interrupt_controller.LookupHandler,  // 30, 0x78 - SSP0
  interrupt_controller.LookupHandler,  // 31, 0x7c - SSP1
  interrupt_controller.LookupHandler,  // 32, 0x80 - PLL0 (Main PLL)
  interrupt_controller.LookupHandler,  // 33, 0x84 - RTC
  interrupt_controller.LookupHandler,  // 34, 0x88 - EINT0
  interrupt_controller.LookupHandler,  // 35, 0x8c - EINT1
  interrupt_controller.LookupHandler,  // 36, 0x90 - EINT2
  interrupt_controller.LookupHandler,  // 37, 0x94 - EINT3
  interrupt_controller.LookupHandler,  // 38, 0x98 - ADC
  interrupt_controller.LookupHandler,  // 39, 0x9c - BOD
  interrupt_controller.LookupHandler,  // 40, 0xA0 - USB
  interrupt_controller.LookupHandler,  // 41, 0xa4 - CAN
  interrupt_controller.LookupHandler,  // 42, 0xa8 - GP DMA
  interrupt_controller.LookupHandler,  // 43, 0xac - I2S
  interrupt_controller.LookupHandler,  // 44, 0xb0 - Ethernet
  interrupt_controller.LookupHandler,  // 45, 0xb4 - SD/MMC card I/F
  interrupt_controller.LookupHandler,  // 46, 0xb8 - Motor Control PWM
  interrupt_controller.LookupHandler,  // 47, 0xbc - Quadrature Encoder
  interrupt_controller.LookupHandler,  // 48, 0xc0 - PLL1 (USB PLL)
  interrupt_controller.LookupHandler,  // 49, 0xc4 - USB Activity interrupt to
                                       // wakeup
  interrupt_controller.LookupHandler,  // 50, 0xc8 - CAN Activity interrupt to
                                       // wakeup
  interrupt_controller.LookupHandler,  // 51, 0xcc - UART4
  interrupt_controller.LookupHandler,  // 52, 0xd0 - SSP2
  interrupt_controller.LookupHandler,  // 53, 0xd4 - LCD
  interrupt_controller.LookupHandler,  // 54, 0xd8 - GPIO
  interrupt_controller.LookupHandler,  // 55, 0xdc - PWM0
  interrupt_controller.LookupHandler,  // 56, 0xe0 - EEPROM
};
```

## Writing .mk file

```Makefile
# Add library source files, typically its just the startup.cpp file
LIBRARY_LPC40XX += $(LIBRARY_DIR)/platforms/lpc40xx/startup.cpp
LIBRARY_LPC40XX += $(LIBRARY_DIR)/peripherals/cortex/interrupt_vector_table.cpp

# (Optional) Add test source files if applicable
TESTS += $(LIBRARY_DIR)/platforms/lpc40xx/startup_test.cpp

# (Optional) Add any files or folders that should NOT be evaluated via the
# linter. System headers and register description files to be ignored.
# .c and .h files are not considered for linting in general.
LINT_FILTER += $(LIBRARY_DIR)/platforms/lpc40xx/special_file.hpp

# This call generates a static library for your platform
# $(eval $(call BUILD_LIBRARY, - calls the build library macros
# liblpc40xx - is the name of the library file created and to be linked in.
#              must be unique.
# LIBRARY_LPC40XX - is the variable holding the list of sources files to
#                   build in to the static library.
$(eval $(call BUILD_LIBRARY,liblpc40xx,LIBRARY_LPC40XX))

# (optional) include additional sub directory make files if applicable
# in this case, the LPC40xx uses an Arm Cortex-M4 so we want to add its
# system headers to our include path.
include $(LIBRARY_DIR)/platforms/arm_cortex/m4/m4.mk

# Defines a means of programming a device in a way that does not utilize jtag
platform-flash:
	@echo
	@bash -c "\
	source $(SJ2_TOOLS_DIR)/nxpprog/modules/bin/activate && \
	python3 $(SJ2_TOOLS_DIR)/nxpprog/nxpprog.py \
	--binary=\"$(BINARY)\" --device=\"$(SJDEV)\" \
	--osfreq=12000000 --baud=115200 --control"
	@echo
```

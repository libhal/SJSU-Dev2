# Simple semihosting output functions for Cortex-M devices

For printf, puts and putchar functions that output directly through the debugging interface using semihosting.
They implement semihosting calls directly so have few dependencies, and they may produce a smaller code size than when using their equivalent built-in functions (particularly trace_puts and trace_putchar).

These files are taken directly from the µOS++ project by Liviu Ionescu, which is MIT-licensed.
They are also used in the excellent GNU ARM Eclipse plugins suite by the same author.

They are provided here to ease their use in other projects that are not based on µOS++.

Details on µOS++:
https://sourceforge.net/p/micro-os-plus/

Details on GNU ARM Eclipse:
https://sourceforge.net/p/gnuarmeclipse/

## Functions
Provides the following functions:
```c
int trace_printf(const char* format, ...);
int trace_puts(const char *s);
int trace_putchar(int c);
```

The semihosting integration is performed in the function call_host called by the above functions.

## Usage
Add inc/ to your include paths and src/ to your build path.

Add `#include "trace.h"` to any files you wish to use the functions in.



Two macros must be defined, `TRACE` and one of the following be determine output method:
* `OS_USE_TRACE_ITM` - ARM's Instrumentation Trace Macrocell (not yet supported by OpenOCD)
* `OS_USE_TRACE_SEMIHOSTING_DEBUG` - Semihosting debug (unbuffered)
* `OS_USE_TRACE_SEMIHOSTING_STDOUT` - Semihosting stdout (buffered)

These can be passed to compilers like gcc directly, for example `-DTRACE -DOS_USE_TRACE_SEMIHOSTING_DEBUG`

If using gcc-arm-embedded toolchain you should add the linker option `--specs=rdimon.specs`

trace_printf uses vsnprintf from stdio.h, so this must be available, and this function will have a larger code size as a result.

### OpenOCD
To enable semihosting in OpenOCD issue the command `arm semihosting enable` or via gdb with `mon arm semihosting enable`.

For example, after successfully connecting to your target with OpenOCD, connect the debugger:
```
arm-none-eabi-gdb -ex "target ext localhost:3333" -ex "mon reset halt" -ex "mon arm semihosting enable" myproject.elf
```

## Disclaimer
This repository was created to make it easier for me to integrate these functions into my own projects for debugging purposes.
I hope they may be useful to others, but there is no guarantee that they will work outside of my own specific setup.

They have been tested on an STM32F0 platform using the STM32F0xx_HAL drivers, and the gcc-arm-embedded toolchain with newlib-nano:
https://launchpad.net/gcc-arm-embedded. I've found them to produce a smaller code size than the built-in output functions used in conjunction with semihosting.

Many thanks to Liviu Ionescu for his great work.

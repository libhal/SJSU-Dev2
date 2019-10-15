Steps to port your microcontroller
===================================

1. Copy this folder and rename it to the name of your system in snake_case.
   Example: lpc40xx or atmega328p
2. Rename example.mk to exactly the name of the platform folder.
   Example:
    * `L0_Platform/lpc40xx/lpc40xx.mk`
    * `L0_Platform/atmega328p/atmega328p.mk`
3. Add the memory map header definitions file into the folder. Be sure to add an
   _c before the extension to indicate that the file is a C file.
   Example:
    * `L0_Platform/example/example_map_c.h`
    * `L0_Platform/lpc40xx/LPC40xx_c.h`
4. Create a map .h or .hpp file and include the _c map file. Follow the
   `L0_Platform/example/example_memory_map.hpp` example.
   Example:
    * `L0_Platform/example/example_map.hpp`
    * `L0_Platform/lpc40xx/LPC40xx.h`
5. Open `startup.cpp` and follow comment instructions in the file.
6. Open `interrupt.cpp` and `interrup.hpp` and follow comment instructions in
   the file.
7. Open `diskio.cpp` and follow comment instructions in the file
   (optional if you aren't using fatfs)
8. Attempt to compile and flash and fix any bugs that occur.
9. Done!!

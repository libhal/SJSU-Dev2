#pragma once

#include "L0_Platform/ram.hpp"

extern "C"
{
  // Declare __libc_init_array function which will be used to run the C++
  // constructors. This is supplied by the compiler, no need to define it
  // yourself.
  // NOLINTNEXTLINE(readability-identifier-naming)
  extern void __libc_init_array(void);
  // Declare The entry point for the application.
  // This will need to be supplied by the developer.
  extern int main();
}

namespace sjsu
{
// 12. Define SystemInitialize(), which is done mostly for you, in order to
// setup the .data, .bss sections, run C++ constructors and setup what ever else
// is required for
inline void SystemInitialize()
{
  // SysInit 1. Transfer data section values from flash to RAM
  //            (ALREADY DONE FOR YOU)
  InitializeDataSection();
  // SysInit 2. Clear BSS section of RAM
  //            This is required because the nano implementation of the
  //            standard C/C++ libraries assumes that the BSS section is
  //            initialized to 0.
  //            (ALREADY DONE FOR YOU)
  InitializeBssSection();
  //  SysInit 3. Initialisation C++ libraries
  //            (ALREADY DONE FOR YOU)
  __libc_init_array();
}
}  // namespace sjsu

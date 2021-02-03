#pragma once

#include "platforms/utility/ram.hpp"
#include "utility/build_info.hpp"

extern "C"
{
  // Declare __libc_init_array function which will be used to run the C++
  // constructors. This is supplied by the compiler, no need to define it
  // yourself.
  // NOLINTNEXTLINE(readability-identifier-naming)
  extern void __libc_init_array(void);
  // The entry point for the application.
  // main() is the entry point for newlib based applications
  extern int main();
  // External declaration for the pointer to the stack top from the linker
  // script
  extern void StackTop(void);
}

namespace sjsu
{
// Sets up the .data, .bss sections, run C++ constructors.
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

  // Checks at compile time if this is a unit test. If it is, then do not
  // attempt to run `__libc_init_array()`
  if constexpr (!build::IsPlatform(build::Platform::host))
  {
    //  SysInit 3. Initialisation C++ libraries
    //            (ALREADY DONE FOR YOU)
    __libc_init_array();
  }
}

// Declaration of an InitializePlatform function that every platform must have
// defined and available at link time. Typically this is defined as [[weak]]
// in the startup.cpp file.
extern void InitializePlatform();
}  // namespace sjsu

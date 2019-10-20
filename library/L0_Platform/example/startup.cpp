// 1. Include needed C headers here
// ---------------------------------

// 2. Include needed C++ headers here
#include <cinttypes>
#include <cstddef>
#include <cstdint>
// ---------------------------------

// 3. Include SJSU-Dev2 headers here
#include "L0_Platform/startup.hpp"
#include "newlib/newlib.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"
// ---------------------------------

// Private namespace to make sure that these do not conflict with other globals
namespace
{
// 4. Define all necessary objects and variables within this namespace.
// ---------------------------------------------------------------------

// 5. Define all necessary function calls here
// --------------------------------------------

// 6. Define an milliseconds Uptime function
// ------------------------------------------
std::chrono::microseconds ExampleUptime()
{
  return 0us;
}
// 7. Define a function for stdout
//    Typically uses UART0 or USB CDC
int StdOut(const char * data, size_t length)
{
  return length;
}
// 8. Define a function for stdin
//    Typically uses UART0 or USB CDC
int StdIn(char * data, size_t length)
{
  return length;
}
}  // namespace

// The entry point for the C++ library startup
extern "C"
{
  // 9. Define a fast thread runtime counter. It cannot be in milliseconds,
  //    preefered is microseconds. The faster the better, but it should also not
  //    overflow very often.
  uint32_t ThreadRuntimeCounter()
  {
    // Return value from a fast register counter.
  }
  // 10. Define a function that will setup the RTOS timer and assign
  //     xPortSysTickHandler as interrupt handler.
  void vPortSetupTimerInterrupt(void)  // NOLINT
  {
  }
}

// 11. Define default instance of InitializePlatform() function.
//     Here you construct everything that is needed to run the micrcontroller.
[[gnu::weak]] void InitializePlatform() {
  // Platform 1. Enable any coprocessors or peripherals that keep the following
  // functions from being executed, for example, activating the floating point
  // unit in the Cortex M4 before running any floating point arithmetic.

  // Platform 2. Set Clock Speed

  // Platform 3. Configure peripheral clocks

  // Platform 4. Setup hardware for host communication

  // Platform 5. Set newlib::SetStdout & newlib::SetStdin callbacks to the
  //             functions defined in startup.7 and startup.8
  sjsu::newlib::SetStdout(StdOut);
  sjsu::newlib::SetStdin(StdIn);
  // Platform 6. Set uptime function to the uptime function defined in startup.6
  sjsu::SetUptimeFunction(ExampleUptime);
}

// Reset entry point for your code.
// Sets up a simple runtime environment and initializes the C/C++ library.

extern "C" void ResetHandler()
{
  // 12. Set stack pointer back up to the top of the stack.
  //
  // The Hyperload bootloader takes up stack space to execute. The Hyperload
  // bootloader function launches this ISR manually, but it never returns thus
  // it never cleans up the memory it uses. To get that memory back, we have
  // to manually move the stack pointers back to the top of stack.

  // 13. Run SystemInitialize() which sets up the .data and .bss sections and
  //     run C++ constructors.
  sjsu::SystemInitialize();

// #pragma ignored "-Wpedantic" to suppress main function call warning
#pragma GCC diagnostic push ignored "-Wpedantic"
  int32_t result = main();
// Enforce the warning after this point
#pragma GCC diagnostic pop
  // main() shouldn't return, but if it does, we'll just enter an infinite
  // loop
  LOG_ERROR("main() returned with value %" PRId32, result);
  sjsu::Halt();
}
// Add section variables. Typically this includes checksums or security lock
// values.
// The .crp section kCrpWord
SJ2_SECTION(".crp") const uint32_t kCrpWord = 0xFFFFFFFF;

// 14. Define an milliseconds Uptime function
// ------------------------------------------
// Place interrupt vector table here. How this is implemented is very platform
// specific. See the manufacturer's startup file for details on what it should
// look like.

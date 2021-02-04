#include <unistd.h>

#include <chrono>
#include <span>

#include "peripherals/interrupt.hpp"
#include "peripherals/system_controller.hpp"
#include "newlib/newlib.hpp"
#include "utility/macros.hpp"
#include "utility/time/time.hpp"

// Private namespace to make sure that these do not conflict with other globals
namespace
{
int LinuxStdOut(std::span<const char> data)
{
  return write(STDOUT_FILENO, data.data(), data.size());
}

int LinuxStdIn(std::span<char> data)
{
  return read(STDIN_FILENO, data.data(), data.size());
}

extern "C" int _write(int, const char * ptr, int length)  // NOLINT
{
  return LinuxStdOut(std::span<const char>(ptr, length));
}

std::chrono::nanoseconds LinuxUptime()
{
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::steady_clock::now().time_since_epoch());
}
}  // namespace

extern "C"
{
  int heap;
  int StackTop;  // NOLINT
  uint32_t ThreadRuntimeCounter()
  {
    return static_cast<uint32_t>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::steady_clock::now().time_since_epoch())
            .count());
  }
}

namespace sjsu
{
class InitializePlatformLinux
{
 public:
  InitializePlatformLinux()
  {
    auto & interrupt_controller =
        sjsu::GetInactive<sjsu::InterruptController>();
    auto & system_controller = sjsu::GetInactive<sjsu::SystemController>();
    sjsu::InterruptController::SetPlatformController(&interrupt_controller);
    sjsu::SystemController::SetPlatformController(&system_controller);
    sjsu::newlib::SetStdout(LinuxStdOut);
    sjsu::newlib::SetStdin(LinuxStdIn);
    sjsu::SetUptimeFunction(LinuxUptime);
  }
};

// You may be wondering how in the world does this work? If you read the newlib
// porting document, you will see that running C++ requires that startup, or
// whatever starts running before main is called, must call __libc_init_array()
// before calling main. This function does a lot of work, and one of its jobs is
// to run all object constructors. The order is not easily determined. Any
// constructor can happen before this constructor, which is why it is a style
// rule to NOT use hardware within a constructor. That includes print statements
// as they may use a UART or USB implementation that may not have been setup
// yet.
//
// Thus, in order to ensure that stdout, stdin and uptime are all set, we create
// a class with a constructor that does all of this work, then we create a
// global instance of that object. The object will be constructed before main is
// called, achieving our desired effect.
//
// We could have tried to overload _start (an assembly tag that indicates the
// start of a program in linux), but _start does a lot of work that we would
// have to recreate. Rather than do that, lets just use the _start given to us
// and come up with a solution that requires far less work.
InitializePlatformLinux startup;
}  // namespace sjsu

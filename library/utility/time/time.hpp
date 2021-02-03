#pragma once

#include <time.h>

#include <cstdint>
#include <cinttypes>
#include <functional>
#include <cstdio>

#include "utility/macros.hpp"
#include "utility/error_handling.hpp"
#include "utility/math/units.hpp"
#include "utility/build_info.hpp"

namespace sjsu
{
/// Definition of an UptimeFunction
using UptimeFunction = std::function<std::chrono::nanoseconds(void)>;

/// A default uptime function that is used for testing or platforms without a
/// means to keep time. It should not be used in production.
///
/// @return the current number of fake uptime nanoseconds that increments each
/// time this function is called.
inline std::chrono::nanoseconds DefaultUptime()
{
  static std::chrono::nanoseconds default_uptime = 0ns;
  default_uptime += 1us;
  return default_uptime;
}

/// Global Uptime function, preset to DefaultUptime() for testing purposes.
/// In general, this function is overwritten by
inline UptimeFunction Uptime = DefaultUptime;  // NOLINT

/// Global count of the time.
inline std::chrono::nanoseconds global_time = 0ns;

/// Returns the system uptime in nanoseconds, do not use this function directly
///
/// @param uptime_function - new system wide uptime function to override the
///        previous one.
inline void SetUptimeFunction(UptimeFunction uptime_function)
{
  Uptime = uptime_function;
}

/// Wait will until the is_done parameter returns true
///
/// @param timeout the maximum amount of time to wait for the is_done to
///        return true.
/// @param is_done will be run in a tight loop until it returns true or the
///        timeout time has elapsed.
/// @returns true when the is_done routine returned true before timeout time
/// elapsed.
inline bool Wait(std::chrono::nanoseconds timeout,
                 std::function<bool()> is_done)
{
  std::chrono::nanoseconds timeout_time;

  if (timeout == std::chrono::nanoseconds::max())
  {
    // TODO(#983): This is a cheap hack to keep overflows from happening, but
    // what if the system hsa been on for a long period of time and a new
    // timeout overflows? This needs to be handled properly.
    timeout_time = timeout;
  }
  else if (timeout == 0ns)
  {
    return false;
  }
  else
  {
    if constexpr (build::IsPlatform(build::Platform::host))
    {
      // NOTE: During host tests the default uptime counter will auto increment
      // by 1, which will resulting in the default uptime being 1ns additional
      // then it should. To counter act the calls to extra calls to Uptime() in
      // this function, we substract 2ns.
      timeout_time = (Uptime() + timeout) - 2us;
    }
    else
    {
      timeout_time = Uptime() + timeout;
    }
  }

  global_time = Uptime();
  while (global_time <= timeout_time)
  {
    if (is_done())
    {
      return true;
    }
    global_time = Uptime();
  }

  return false;
}

/// Overload of `Wait` that merely takes a timeout.
///
/// @param timeout - the amount of time to wait.
/// @return always returns std::errc::timed_out
inline bool Wait(std::chrono::nanoseconds timeout)
{
  return Wait(timeout, []() -> bool { return false; });
}

/// Declare an external linkage to the linux nanosleep() function. This is
/// not needed for linux builds but is required to keep the ARM compiler from
/// stating that the nanosleep() function does not exist, as it is "ifdef"
/// out in the header file.
///
/// During the linking stage, since the constexpr if will fail, this path
/// will be removed form the code, and thus no linking errors due to an
/// undefined function call.
extern int nanosleep(const timespec *, const timespec *);  // NOLINT

/// Delay the system for a duration of time
inline void Delay(std::chrono::nanoseconds delay_time)
{
  // For Linux Systems since using Wait (which will busy loop) will drive the
  // CPU utilization up to maximum.
  if constexpr (build::kPlatform == build::Platform::linux)
  {
    // Needs to use the linux timespec structure to pass into nanosleep
    timespec ts;

    // Error code return value to be checked if the sleep has finished.
    int res;

    // Convert delay time to seconds to be stored into timespec structure.
    // Using time mod 1000000000 to trim off the time greater than a second.
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(delay_time);
    ts.tv_sec    = static_cast<decltype(ts.tv_sec)>(seconds.count());
    ts.tv_nsec =
        static_cast<decltype(ts.tv_nsec)>(delay_time.count() % 1000000000);

    do
    {
      res = nanosleep(&ts, &ts);
    } while (res);
  }
  else
  {
    // For all other systems use the Wait function to loop until time is up.
    Wait(delay_time);
  }
}

/// Halt system by putting it into infinite loop
inline void Halt()
{
  while (true)
  {
    continue;
  }
}
}  // namespace sjsu

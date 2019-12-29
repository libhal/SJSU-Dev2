#pragma once

#include <cstdint>
#include <cinttypes>
#include <functional>
#include <cstdio>

#include "utility/macros.hpp"
#include "utility/status.hpp"
#include "utility/units.hpp"
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
inline Status Wait(std::chrono::nanoseconds timeout,
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
    return Status::kTimedOut;
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

  Status status = Status::kTimedOut;
  while (Uptime() <= timeout_time)
  {
    if (is_done())
    {
      status = Status::kSuccess;
      break;
    }
  }
  return status;
}

/// Overload of `Wait` that merely takes a timeout.
///
/// @param timeout - the amount of time to wait.
/// @return always returns Status::kTimedOut
inline Status Wait(std::chrono::nanoseconds timeout)
{
  return Wait(timeout, []() -> bool { return false; });
}

/// Delay the system for a duration of time
inline void Delay(std::chrono::nanoseconds delay_time)
{
  Wait(delay_time);
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

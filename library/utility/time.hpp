#pragma once

#include <cstdint>

#include "utility/macros.hpp"
#include "utility/status.hpp"
#include "utility/units.hpp"
#include "utility/build_info.hpp"

namespace sjsu
{
// Definition of an UptimeFunction
using UptimeFunction = std::chrono::microseconds (*)();

inline std::chrono::microseconds DefaultUptime()
{
  static std::chrono::microseconds default_uptime = 0us;
  return default_uptime++;
}

inline UptimeFunction Uptime = DefaultUptime;  // NOLINT

// Returns the system uptime in microseconds, do not use this function directly
inline void SetUptimeFunction(UptimeFunction uptime_function)
{
  Uptime = uptime_function;
}
// Wait will until the is_done parameter returns true
//
// @param timeout the maximum amount of time to wait for the is_done to
//        return true.
// @param is_done will be run in a tight loop until it returns true or the
//        timeout time has elapsed.
template <typename F>
[[gnu::always_inline]] inline Status Wait(std::chrono::microseconds timeout,
                                          F is_done);
template <typename F>
inline Status Wait(std::chrono::microseconds timeout, F is_done)
{
  std::chrono::microseconds timeout_time;
  if (timeout == std::chrono::microseconds::max())
  {
    timeout_time = timeout;
  }
  else
  {
    timeout_time = Uptime() + timeout;
  }

  Status status = Status::kTimedOut;
  while (Uptime() < timeout_time)
  {
    if (is_done())
    {
      status = Status::kSuccess;
      break;
    }
  }
  return status;
}

inline Status Wait(std::chrono::microseconds timeout)
{
  return Wait(timeout, []() -> bool { return false; });
}

// Delay the system for a duration of time
inline void Delay(std::chrono::microseconds delay_time)
{
  if constexpr (sjsu::build::kTarget == sjsu::build::Target::HostTest)
  {
    return;
  }
  else
  {
    Wait(delay_time);
  }
}
// Halt system by putting it into infinite loop
[[gnu::always_inline]] inline void Halt()
{
  while (true)
  {
    continue;
  }
}
}  // namespace sjsu

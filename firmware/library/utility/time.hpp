#pragma once

#include <cstdint>

#include "utility/macros.hpp"
#include "utility/status.hpp"

// Max wait time in nanoseconds.
constexpr uint64_t kMaxWait = 0xFFFFFFFFFFFFFFFF;

namespace
{
// uptime in nanoseconds
inline uint64_t uptime = 0;
// Returns the system uptime in nanoseconds, do not use this function directly
[[gnu::always_inline]] inline uint64_t DefaultUptime()
{
  return uptime;
}
}  // namespace

using UptimeFunction = uint64_t (*)();
// Returns the system uptime in nanoseconds.
inline UptimeFunction Uptime = DefaultUptime;  // NOLINT
inline void SetUptimeFunction(UptimeFunction uptime_function)
{
  Uptime = uptime_function;
}
// Get system uptime in milliseconds as a 64-bit integer
inline uint64_t Milliseconds()
{
  return Uptime() / 1'000;
}
// Get system uptime in seconds as a 64-bit integer
inline uint64_t Seconds()
{
  return Uptime() / 1'000'000;
}
// Wait will until the is_done parameter returns true
//
// @param timeout the maximum amount of time to wait for the is_done to
//        return true.
// @param is_done will be run in a tight loop until it returns true or the
//        timeout time has elapsed.
template <typename F>
[[gnu::always_inline]] inline Status Wait(uint64_t timeout, F is_done);
template <typename F>
inline Status Wait(uint64_t timeout, F is_done)
{
  uint64_t timeout_time = 0;
  if (timeout == kMaxWait)
  {
    timeout_time = kMaxWait;
  }
  else
  {
    timeout_time = Milliseconds() + timeout;
  }

  Status status = Status::kTimedOut;
  while (Milliseconds() < timeout_time)
  {
    if (is_done())
    {
      status = Status::kSuccess;
      break;
    }
  }
  return status;
}

inline Status Wait(uint64_t timeout)
{
  return Wait(timeout, []() -> bool { return false; });
}

// Delay the system for a duration of time
inline void Delay([[maybe_unused]] uint64_t delay_time_ms)
{
#if defined(HOST_TEST)
  return;
#else
  Wait(delay_time_ms);
#endif  // HOST_TEST
}
// Halt system by putting it into infinite loop
SJ2_FUNCTION_INLINE(SJ2_IGNORE_STACK_TRACE(inline void Halt()));
inline void Halt()
{
  while (true)
  {
    continue;
  }
}

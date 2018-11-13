#pragma once

#include <cstdint>

#include "L2_Utilities/macros.hpp"
#include "L2_Utilities/status.hpp"

// Max wait time in nanoseconds.
constexpr uint64_t kMaxWait = 0xFFFFFFFFFFFFFFFF;
// uptime in nanoseconds
inline uint64_t uptime = 0;
// Halt system by putting it into infinite loop
SJ2_FUNCTION_INLINE(SJ2_IGNORE_STACK_TRACE(inline void Halt()));
inline void Halt()
{
  while (true)
  {
    continue;
  }
}
// Returns the system uptime in nanoseconds
inline uint64_t Uptime()
{
  return uptime;
}
// Increment Uptime by 1 millisecond
inline void IncrementUptimeMs()
{
  uptime += 1000;
}
// Get system uptime in milliseconds as a 64-bit integer
inline uint64_t Milliseconds()
{
  return uptime / 1000;
}
// Wait will until the is_done parameter returns true
//
// @param timeout the maximum amount of time to wait for the is_done to
//        return true.
// @param is_done will be run in a tight loop until it returns true or the
//        timeout time has elapsed.
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

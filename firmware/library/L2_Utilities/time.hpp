#pragma once

#include <chrono>

#include "L2_Utilities/macros.hpp"
#include "L2_Utilities/status.hpp"

constexpr std::chrono::milliseconds kMaxWait(std::chrono::milliseconds::max());

static inline std::chrono::nanoseconds uptime(0);

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
inline std::chrono::nanoseconds Uptime()
{
  return uptime;
}
// Increment Uptime by 1 millisecond
inline void IncrementUptimeMs()
{
  uptime += std::chrono::milliseconds(1);
}
// Get system uptime in milliseconds as a 64-bit integer
inline int64_t Milliseconds()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(uptime).count();
}
// Wait will until the is_done parameter returns true
//
// @param timeout the maximum amount of time to wait for the is_done to
//        return true.
// @param is_done will be run in a tight loop until it returns true or the
//        timeout time has elapsed.
SJ2_FUNCTION_INLINE(template <typename F>
                    inline Status Wait(std::chrono::milliseconds timeout,
                                       F is_done));
template <typename F>
inline Status Wait(std::chrono::milliseconds timeout, F is_done)
{
  std::chrono::milliseconds current_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(Uptime());
  std::chrono::milliseconds timeout_time(0);
  if (timeout == kMaxWait)
  {
    timeout_time = kMaxWait;
  }
  else
  {
    timeout_time = current_time + timeout;
  }

  Status status = Status::kTimedOut;
  while (current_time < timeout_time)
  {
    if (is_done())
    {
      status = Status::kSuccess;
      break;
    }
    current_time =
        std::chrono::duration_cast<std::chrono::milliseconds>(Uptime());
  }
  return status;
}

inline Status Wait(std::chrono::milliseconds timeout)
{
  return Wait(timeout, []() -> bool { return false; });
}

// Delay the system for a duration of time
inline void Delay(uint32_t delay_time_ms)
{
#if defined(HOST_TEST)
  SJ2_USED(delay_time_ms);
  return;
#else
  Wait(std::chrono::milliseconds(delay_time_ms));
#endif  // HOST_TEST
}

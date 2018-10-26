#pragma once

#include <chrono>

#include "L2_Utilities/macros.hpp"
#include "L2_Utilities/status.hpp"

constexpr std::chrono::milliseconds kMaxWait(std::chrono::milliseconds::max());

// Halt system by putting it into infinite loop
void Halt();
// Returns the system uptime in nanoseconds
std::chrono::nanoseconds Uptime();
// Increment Uptime by 1 millisecond
void IncrementUptimeMs();
// Get system uptime in milliseconds as a 64-bit integer
int64_t Milliseconds();
// Delay the system for a duration of time
void Delay(uint32_t delay_time_ms);
// Wait will until the is_done parameter returns true
//
// @param timeout the maximum amount of time to wait for the is_done to
//        return true.
// @param is_done will be run in a tight loop until it returns true or the
//        timeout time has elapsed.
template<typename F>
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

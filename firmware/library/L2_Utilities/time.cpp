#include "L2_Utilities/time.hpp"

#include <chrono>

namespace
{
std::chrono::nanoseconds uptime(0);
}  // namespace

void Halt()
{
  while (true)
  {
    continue;
  }
}

int64_t Milliseconds()
{
  return std::chrono::duration_cast<std::chrono::milliseconds>(uptime).count();
}

std::chrono::nanoseconds Uptime()
{
  return uptime;
}

void IncrementUptimeMs()
{
  uptime += std::chrono::milliseconds(1);
}

void Delay(uint32_t delay_time_ms)
{
#if defined(HOST_TEST)
  SJ2_USED(delay_time_ms);
  return;
#else
  Wait(std::chrono::milliseconds(delay_time_ms));
#endif  // HOST_TEST
}

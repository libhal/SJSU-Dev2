#include <inttypes.h>
#include <cstdint>
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  LOG_INFO("Delay Application Starting...");

  LOG_INFO(
      "This example merely prints a statement every second using the delay "
      "function.");

  uint32_t counter = 0;

  while (true)
  {
    LOG_INFO("[%lu] Hello, World! (microseconds = %lu)",
             counter++,
             static_cast<uint32_t>(sjsu::Uptime().count()));
    sjsu::Delay(1s);
  }
  return 0;
}

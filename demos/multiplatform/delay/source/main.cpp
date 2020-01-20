#include <cinttypes>
#include <cstdint>

#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  LOG_INFO("Delay Application Starting...");

  LOG_INFO(
      "This example merely prints a statement every second using the delay "
      "function.");

  LOG_INFO(
      "Notice that the uptime does not increase by 1 second but by a little "
      "more then that. This is due to the fact that we delay for a whole "
      "second, but it takes time to print each statement.");

  int counter = 0;

  while (true)
  {
    LOG_INFO("[%d] Uptime = %" PRId64 "ns", counter++, sjsu::Uptime().count());
    sjsu::Delay(1s);
  }
  return 0;
}

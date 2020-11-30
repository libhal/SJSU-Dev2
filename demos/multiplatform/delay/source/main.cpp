#include <cinttypes>
#include <cstdint>

#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Delay Application Starting...");

  sjsu::LogInfo("This demo prints a statement every second using Delay(1s).");
  sjsu::LogInfo(
      "Notice that the uptime does not increase by 1 second but by a little "
      "more then that.");
  sjsu::LogInfo(
      "This is due to the fact that we delay for a whole second, but it takes "
      "time to print each statement.");

  int counter = 0;

  while (true)
  {
    sjsu::LogInfo(
        "[%d] Uptime = %" PRId64 "ns", counter++, sjsu::Uptime().count());
    sjsu::Delay(1s);
  }
  return 0;
}

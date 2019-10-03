#include <cstdint>

#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  LOG_INFO("Staring Hello World Application");
  while (true)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      sjsu::Log().info("Hello World 0x%X", i);
      sjsu::Delay(500ms);
    }
  }

  return -1;
}

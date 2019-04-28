#include <project_config.hpp>

#include <cstdint>
#include <iterator>

#include "L2_HAL/boards/sjtwo.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  LOG_INFO("Staring Hello World Application");
  LOG_INFO("Initializing LEDs...");
  sjtwo::Leds().Initialize();
  LOG_INFO("LEDs Initialized!");
  while (true)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      LOG_INFO("Hello World 0x%X", i);
      sjtwo::Leds().SetAll(i);
      sjsu::Delay(500);
    }
  }

  return 0;
}

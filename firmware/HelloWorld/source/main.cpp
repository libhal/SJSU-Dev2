#include <project_config.hpp>

#include <cstdint>
#include <iterator>

#include "L2_HAL/displays/led/onboard_led.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main(void)
{
  LOG_INFO("Staring Hello World Application");
  LOG_INFO("Initializing LEDs...");
  OnBoardLed leds;
  leds.Initialize();
  LOG_INFO("LEDs Initialized!");

  while (true)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      LOG_INFO("Hello World 0x%X", i);
      leds.SetAll(i);
      Delay(500);
    }
  }

  return 0;
}

#include <project_config.hpp>

#include <cstdint>
#include <iterator>

#include "L2_HAL/displays/led/onboard_led.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main(void)
{
  DEBUG_PRINT("Staring Hello World Application");
  DEBUG_PRINT("Initializing LEDs...");
  OnBoardLed leds;
  leds.Initialize();
  DEBUG_PRINT("LEDs Initialized! %f", 1234.123456);

  while (true)
  {
    for (uint8_t i = 0; i < 15; i++)
    {
      DEBUG_PRINT("Hello World 0x%X", i);
      leds.SetAll(i);
      Delay(500);
    }
  }
  return 0;
}

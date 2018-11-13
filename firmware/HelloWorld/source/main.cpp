#include <project_config.hpp>

#include <cstdint>
#include <iterator>

#include "L2_Utilities/log.hpp"
#include "L2_Utilities/time.hpp"
#include "L3_HAL/onboard_led.hpp"

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

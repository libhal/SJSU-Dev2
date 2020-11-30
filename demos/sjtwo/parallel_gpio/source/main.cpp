#include <iterator>

#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L2_HAL/io/parallel_bus/parallel_gpio.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Staring Parallel Gpio Application");

  sjsu::LogInfo("Creating gpio (led) objects...");

  sjsu::LogInfo("Creating ParallelGpio object using led Gpios...");

  sjsu::lpc40xx::Gpio led0(2, 3);
  sjsu::lpc40xx::Gpio led1(1, 26);
  sjsu::lpc40xx::Gpio led2(1, 24);
  sjsu::lpc40xx::Gpio led3(1, 18);
  std::array<sjsu::Gpio *, 4> leds = { &led0, &led1, &led2, &led3 };

  sjsu::ParallelGpio parallel_leds(leds);

  parallel_leds.Initialize();
  parallel_leds.Enable();

  while (true)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      sjsu::LogInfo("Displaying 0x%X in binary on leds", i);
      // Each LED above the 4 buttons on the SJTwo is active low, meaning it
      // turns on when the pin is set to state LOW. In order to make an LED
      // shine when the corrissponding bit is 1, I need to invert the value of
      // i. To do this, we use the bitwise invert operator ~
      parallel_leds.Write(~i);
      sjsu::Delay(500ms);
    }
  }
}

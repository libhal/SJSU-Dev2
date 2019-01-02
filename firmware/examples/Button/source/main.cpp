#include "L2_Utilities/log.hpp"
#include "L2_Utilities/time.hpp"
#include "L3_HAL/button.hpp"
#include "L3_HAL/onboard_led.hpp"

int main(void)
{
  LOG_INFO("Button application starting...");
  Button button0(1, 19);
  Button button1(1, 15);
  Button button2(0, 30);
  Button button3(0, 29);

  OnBoardLed leds;

  leds.Initialize();
  button0.Initialize();
  button1.Initialize();
  button2.Initialize();
  button3.Initialize();

  while (true)
  {
    if (button0.Released())
    {
      leds.Toggle(0);
    }
    if (button1.Pressed())
    {
      leds.Toggle(1);
    }
    if (button2.Released())
    {
      leds.Toggle(2);
    }
    if (button3.Released())
    {
      leds.Toggle(3);
    }
    Delay(1);
  }

  return 0;
}

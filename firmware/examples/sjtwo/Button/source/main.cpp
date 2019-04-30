#include "L2_HAL/displays/led/onboard_led.hpp"
#include "L2_HAL/switches/button.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  LOG_INFO("Button application starting...");
  sjsu::Button button0(1, 19);
  sjsu::Button button1(1, 15);
  sjsu::Button button2(0, 30);
  sjsu::Button button3(0, 29);

  sjsu::OnBoardLed leds;

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
    sjsu::Delay(1);
  }

  return 0;
}

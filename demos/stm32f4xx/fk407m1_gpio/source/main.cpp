#include <cstdio>

#include "peripherals/stm32f4xx/gpio.hpp"
#include "utility/time/time.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Starting FK407M1 Gpio Application...");
  ///////////// Setup LED GPIO /////////////
  sjsu::stm32f4xx::Gpio & led = sjsu::stm32f4xx::GetGpio<'C', 13>();
  led.Initialize();
  led.SetAsOutput();

  sjsu::stm32f4xx::Gpio & button = sjsu::stm32f4xx::GetGpio<'A', 15>();
  button.GetPin().settings.PullUp();
  button.Initialize();
  button.SetAsInput();

  while (true)
  {
    // If button is not pressed (true), repeat blinking led
    // If the button is held down (false), stop blinking.
    if (button.Read())
    {
      led.SetLow();
      sjsu::Delay(100ms);

      led.SetHigh();
      sjsu::Delay(100ms);
    }
    else
    {
      led.SetHigh();
    }
  }
  return 0;
}

#include <cstdio>

#include "peripherals/stm32f10x/gpio.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"

int main()
{
  sjsu::LogInfo(
      "Starting GPIO Application (targeted for Super Bluepill board)...");
  sjsu::stm32f10x::Gpio & key_button = sjsu::stm32f10x::GetGpio<'A', 8>();
  sjsu::stm32f10x::Gpio & led        = sjsu::stm32f10x::GetGpio<'A', 1>();

  // The STM32F10x has an interesting binding between input/output and pin
  // configuration, so this must happen after enable.
  key_button.GetPin().settings.PullUp();
  key_button.Initialize();
  key_button.SetAsInput();
  sjsu::LogInfo("Configured PA8 as input with internal pull up resistor");

  // Another way to configure the input or output of a pin is to use the
  // method, SetDirection.
  led.Initialize();
  led.SetAsOutput();
  led.SetHigh();

  sjsu::LogInfo("Configure PA1 as output and set to output HIGH");
  sjsu::LogInfo("Press the KEY button to turn on the LED.");

  while (true)
  {
    if (key_button.Read())
    {
      led.SetHigh();
    }
    else
    {
      led.SetLow();
    }
  }

  return 0;
}

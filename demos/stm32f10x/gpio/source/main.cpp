#include <cstdio>

#include "L1_Peripheral/stm32f10x/gpio.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo(
      "Starting GPIO Application (targeted for Super Bluepill board)...");
  sjsu::stm32f10x::Gpio key_button('A', 8);
  sjsu::stm32f10x::Gpio led('A', 1);

  key_button.Initialize();
  key_button.Enable();
  key_button.SetAsInput();
  // The STM32F10x has an interesting binding between input/output and pin
  // configuration, so this must happen after enable.
  key_button.GetPin().ConfigurePullUp();
  sjsu::LogInfo("Configured PA8 as input with internal pull up resistor");

  // Another way to configure the input or output of a pin is to use the
  // method, SetDirection.
  led.Initialize();
  led.Enable();
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

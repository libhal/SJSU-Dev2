#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L2_HAL/boards/sjtwo.hpp"
#include "L2_HAL/switches/button.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  LOG_INFO("Button application starting...");
  sjsu::lpc40xx::Gpio button_gpio0(1, 19);
  sjsu::lpc40xx::Gpio button_gpio1(1, 15);
  sjsu::lpc40xx::Gpio button_gpio2(0, 30);
  sjsu::lpc40xx::Gpio button_gpio3(0, 29);

  sjsu::Button button0(button_gpio0);
  sjsu::Button button1(button_gpio1);
  sjsu::Button button2(button_gpio2);
  sjsu::Button button3(button_gpio3);

  sjtwo::led0.SetAsOutput();
  sjtwo::led1.SetAsOutput();
  sjtwo::led2.SetAsOutput();
  sjtwo::led3.SetAsOutput();

  button0.Initialize();
  button1.Initialize();
  button2.Initialize();
  button3.Initialize();

  while (true)
  {
    if (button0.Released())
    {
      sjtwo::led0.Toggle();
    }
    if (button1.Pressed())
    {
      sjtwo::led1.Toggle();
    }
    if (button2.Released())
    {
      sjtwo::led2.Toggle();
    }
    if (button3.Released())
    {
      sjtwo::led3.Toggle();
    }
    sjsu::Delay(1ms);
  }

  return 0;
}

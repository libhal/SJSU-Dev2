#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L2_HAL/boards/sjtwo.hpp"
#include "L2_HAL/switches/button.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Button application starting...");

  sjsu::lpc40xx::Gpio button_gpio0(1, 19);
  sjsu::lpc40xx::Gpio button_gpio1(1, 15);
  sjsu::lpc40xx::Gpio button_gpio2(0, 30);
  sjsu::lpc40xx::Gpio button_gpio3(0, 29);

  // Early initialization of gpio2 and gpio3 to set their pull up resistors
  button_gpio0.Initialize();
  button_gpio1.Initialize();

  button_gpio0.GetPin().ConfigurePullDown();
  button_gpio1.GetPin().ConfigurePullDown();

  button_gpio0.Enable();
  button_gpio1.Enable();

  sjsu::Button button0(button_gpio0);
  sjsu::Button button1(button_gpio1);
  sjsu::Button button2(button_gpio2);
  sjsu::Button button3(button_gpio3);

  sjsu::LogInfo("Initializing LEDs and Buttons...");
  sjtwo::led0.Initialize();
  sjtwo::led1.Initialize();
  sjtwo::led2.Initialize();
  sjtwo::led3.Initialize();

  button0.Initialize();
  button1.Initialize();
  button2.Initialize();
  button3.Initialize();

  sjsu::LogInfo("Enabling LEDs and Buttons...");
  sjtwo::led0.Enable();
  sjtwo::led1.Enable();
  sjtwo::led2.Enable();
  sjtwo::led3.Enable();

  button0.Enable();
  button1.Enable();
  button2.Enable();
  button3.Enable();

  sjsu::LogInfo("Set LEDs as outputs...");

  sjtwo::led0.SetAsOutput();
  sjtwo::led1.SetAsOutput();
  sjtwo::led2.SetAsOutput();
  sjtwo::led3.SetAsOutput();

  sjsu::LogInfo("Button 0: Press and release to toggle led 0...");
  sjsu::LogInfo("Button 1: Press to toggle led 1...");
  sjsu::LogInfo("Button 2: Press and release to toggle led 2...");
  sjsu::LogInfo("Button 2: Press and release to toggle led 3...");

  while (true)
  {
    // Button is a polling action, so you need to check often in order to catch
    // the button press/release event.
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

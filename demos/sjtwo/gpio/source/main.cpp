#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "utility/log.hpp"

int main()
{
  // This example code uses the on-board LED and switch, labeled LED0
  // and SW0 respectively on the schematic.  In this source file, there is a
  // pdf labled "SW&LED_Ckts", showing how the circuits that are being used
  // are constructed.
  LOG_INFO("Gpio application starting...");
  sjsu::lpc40xx::Gpio p1_19(1, 19);
  sjsu::lpc40xx::Gpio p2_03(2, 3);

  p1_19.GetPin().SetPull(sjsu::Pin::Resistor::kPullDown);
  p1_19.SetAsInput();
  LOG_INFO("Configured port 1, pin 19 as input");

  // Another way to configure the input or output of a pin is to use the
  // method, SetDirection.
  p2_03.SetDirection(sjsu::Gpio::Direction::kOutput);
  LOG_INFO("Configure port 2, pin 3 as output");

  LOG_INFO(
      "Observe how the output of LED0 changes when the input of SW0 is "
      "pressed");
  while (true)
  {
    if (p1_19.Read())
    {
      p2_03.SetLow();
    }
    else
    {
      // Another way to set the output of a pin high or low would be to
      // use the Set method, as demonstrated below.
      p2_03.Set(sjsu::Gpio::State::kHigh);
    }
  }

  return 0;
}

#include "peripherals/stm32f10x/gpio.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Staring GPIO Interrupt Application...");

  sjsu::stm32f10x::Gpio pin0('A', 8);
  sjsu::stm32f10x::Gpio pin1('B', 10);
  sjsu::stm32f10x::Gpio pin2('B', 12);

  pin0.GetPin().settings.PullUp();
  pin1.GetPin().settings.PullUp();
  pin2.GetPin().settings.PullUp();

  pin0.Initialize();
  pin1.Initialize();
  pin2.Initialize();

  pin0.SetAsInput();
  pin1.SetAsInput();
  pin2.SetAsInput();

  sjsu::LogInfo("Setup PA8 to interrupt on only Falling edges...");
  pin0.AttachInterrupt([]() { sjsu::LogInfo("PA8 interrupt!"); },
                       sjsu::Gpio::Edge::kFalling);

  // Using shorthand function to enable the attach interrupts to this pin for
  // both rising edges.
  sjsu::LogInfo("Setup PB10 to interrupt on only Rising edges...");
  pin1.OnRisingEdge([]() { sjsu::LogInfo("PB10 interrupt!"); });

  // Using shorthand function to enable the attach interrupts to this pin for
  // both rising and falling edges.
  sjsu::LogInfo("Setup PB12 to interrupt on Rising and Falling edges...");
  pin2.OnChange([]() { sjsu::LogInfo("PB12 interrupt!"); });

  sjsu::LogInfo(
      "All of the pins are currently pulled high using an internal pull-up "
      "resistor.");
  sjsu::LogInfo(
      "Create a rising or falling edge by connecting any of the pins "
      "to GND using a jumper to trigger Interrupt.");

  sjsu::LogInfo("Halting program to keep main from exiting.");
  sjsu::Halt();

  return 0;
}

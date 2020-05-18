#include "L1_Peripheral/stm32f10x/gpio.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Staring GPIO Interrupt Application...");

  sjsu::stm32f10x::Gpio pin0('B', 8);
  sjsu::stm32f10x::Gpio pin1('B', 10);
  sjsu::stm32f10x::Gpio pin2('B', 12);

  pin0.SetAsInput();
  pin0.GetPin().PullUp();
  pin1.SetAsInput();
  pin1.GetPin().PullUp();
  pin2.SetAsInput();
  pin2.GetPin().PullUp();

  sjsu::LogInfo("Setup PB[8] to interrupt on only Falling edges...");
  pin0.AttachInterrupt([]() { sjsu::LogInfo("PB[8] interrupt!"); },
                       sjsu::Gpio::Edge::kEdgeFalling);

  sjsu::LogInfo("Setup PB[10] to interrupt on only Rising edges...");
  pin1.AttachInterrupt([]() { sjsu::LogInfo("PB[10] interrupt!"); },
                       sjsu::Gpio::Edge::kEdgeRising);

  sjsu::LogInfo("Setup PB[12] to interrupt on Rising and Falling edges...");
  pin2.AttachInterrupt([]() { sjsu::LogInfo("PB[12] interrupt!"); },
                       sjsu::Gpio::Edge::kEdgeBoth);

  sjsu::LogInfo(
      "All of the pins are currently pulled high using an internal pull-up "
      "resistor. Create a rising or falling edge by connecting any of the pins "
      "to GND using a jumper to trigger Interrupt.");

  sjsu::LogInfo("Halting program to keep main from exiting.");
  sjsu::Halt();

  return 0;
}

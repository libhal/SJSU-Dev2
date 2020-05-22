#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Staring GPIO Interrupt Application...\n");

  sjsu::lpc40xx::Gpio pin0(0, 15);
  sjsu::lpc40xx::Gpio pin1(2, 9);
  sjsu::lpc40xx::Gpio pin2(0, 30);
  sjsu::lpc40xx::Gpio pin3(0, 29);

  pin0.SetAsInput();
  pin0.GetPin().PullUp();
  pin1.SetAsInput();
  pin1.GetPin().PullUp();
  pin2.SetAsInput();
  pin2.GetPin().PullUp();
  pin3.SetAsInput();
  pin3.GetPin().PullUp();

  sjsu::LogInfo("Setup P0[15] to interrupt on only Falling edges...");
  pin0.AttachInterrupt([]() { sjsu::LogInfo("P0[15] interrupt!"); },
                       sjsu::Gpio::Edge::kEdgeFalling);

  sjsu::LogInfo("Setup P2[9] to interrupt on only Rising edges...");
  pin1.OnRisingEdge([]() { sjsu::LogInfo("P2[9] interrupt!"); });

  sjsu::LogInfo("Setup P0[29] to interrupt on Rising and Falling edges...");
  pin2.OnChange([]() { sjsu::LogInfo("P0[29] interrupt!"); });

  sjsu::LogInfo("Setup P0[30] to interrupt on Rising and Falling edges...");
  pin2.OnChange([]() { sjsu::LogInfo("P0[30] interrupt!"); });

  sjsu::LogInfo(
      "All of the pins are currently pulled high using an internal pull-up "
      "resistor. Create a rising or falling edge by connecting any of the pins "
      "to GND using a jumper to trigger Interrupt.");

  sjsu::LogInfo("Halting program to keep main from exiting.");
  sjsu::Halt();

  return 0;
}

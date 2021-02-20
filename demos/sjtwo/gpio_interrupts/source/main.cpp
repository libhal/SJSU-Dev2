#include "peripherals/lpc40xx/gpio.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Staring GPIO Interrupt Application...\n");

  sjsu::lpc40xx::Gpio & pin0 = sjsu::lpc40xx::GetGpio<0, 15>();
  sjsu::lpc40xx::Gpio & pin1 = sjsu::lpc40xx::GetGpio<2, 9>();
  sjsu::lpc40xx::Gpio & pin2 = sjsu::lpc40xx::GetGpio<0, 29>();
  sjsu::lpc40xx::Gpio & pin3 = sjsu::lpc40xx::GetGpio<0, 30>();

  // Connect internal pull up for each pin
  pin0.GetPin().settings.PullUp();
  pin1.GetPin().settings.PullUp();
  pin2.GetPin().settings.PullUp();
  pin3.GetPin().settings.PullUp();

  // Initialize all pins
  pin0.Initialize();
  pin1.Initialize();
  pin2.Initialize();
  pin3.Initialize();

  // Set as an input
  pin0.SetAsInput();
  pin1.SetAsInput();
  pin2.SetAsInput();
  pin3.SetAsInput();


  sjsu::LogInfo("Setup P0[15] to interrupt on only Falling edges...");
  pin0.AttachInterrupt([]() { sjsu::LogInfo("P0[15] interrupt!"); },
                       sjsu::Gpio::Edge::kFalling);

  sjsu::LogInfo("Setup P2[9] to interrupt on only Rising edges...");
  pin1.OnRisingEdge([]() { sjsu::LogInfo("P2[9] interrupt!"); });

  sjsu::LogInfo("Setup P0[29] to interrupt on Rising edges...");
  pin2.OnRisingEdge([]() { sjsu::LogInfo("P0[29] interrupt!"); });

  sjsu::LogInfo("Setup P0[30] to interrupt on Rising and Falling edges...");
  pin3.OnChange([]() { sjsu::LogInfo("P0[30] interrupt!"); });

  sjsu::LogInfo(
      "All of the pins are currently pulled high using an internal pull-up "
      "resistor.");
  sjsu::LogInfo(
      "Create a rising or falling edge by connecting any of the pins "
      "to GND using a jumper to trigger Interrupt.");

  sjsu::LogInfo("Preventing the program from exiting...");
  sjsu::Halt();

  return 0;
}

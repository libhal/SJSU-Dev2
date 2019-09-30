#include <project_config.hpp>

#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "utility/log.hpp"

void Port0Pin15ISR()
{
  LOG_INFO("Pin0_15_ISR");
}
void Port2Pin9ISR()
{
  LOG_INFO("Pin2_9_ISR");
}
void Port0Pin29ISR()
{
  LOG_INFO("Switch 3 has been pressed!");
}
void Port0Pin30ISR()
{
  LOG_INFO("Switch 2 has been pressed!");
}

int main()
{
  LOG_INFO("Staring GPIO Interrupt Application...\n");

  LOG_INFO("Setting up pin0_15 to interrupt on Rising and Falling edges...");
  sjsu::lpc40xx::Gpio pin0_15(0, 15);
  pin0_15.GetPin().SetPull(sjsu::Pin::Resistor::kPullUp);
  pin0_15.AttachInterrupt(Port0Pin15ISR, sjsu::Gpio::Edge::kEdgeBoth);
  pin0_15.EnableInterrupts();
  LOG_INFO("Use a jumper wire with gnd and pin0_15 to trigger Interrupt.\n");

  LOG_INFO("Setting up pin2_9 to interrupt on Rising and Falling edges...");
  sjsu::lpc40xx::Gpio pin2_9(2, 9);
  pin2_9.GetPin().SetPull(sjsu::Pin::Resistor::kPullUp);
  pin2_9.AttachInterrupt(Port2Pin9ISR, sjsu::Gpio::Edge::kEdgeBoth);
  LOG_INFO("Use a jumper wire with gnd and pin2_9 to trigger Interrupt.\n");

  LOG_INFO(
      "Setting up pin0_30 (Switch 2) to interrupt on only Rising edges...");
  sjsu::lpc40xx::Gpio pin0_30(0, 30);
  pin0_30.GetPin().SetPull(sjsu::Pin::Resistor::kPullUp);
  pin0_30.AttachInterrupt(Port0Pin30ISR, sjsu::Gpio::Edge::kEdgeRising);
  LOG_INFO("Press Switch 2 to trigger Interrupt for pin0_30.\n");

  LOG_INFO(
      "Setting up pin0_29 (Switch 3) to interrupt on only Falling edges...");
  sjsu::lpc40xx::Gpio pin0_29(0, 29);
  pin0_29.GetPin().SetPull(sjsu::Pin::Resistor::kPullUp);
  pin0_29.AttachInterrupt(Port0Pin29ISR, sjsu::Gpio::Edge::kEdgeFalling);
  LOG_INFO("Press Switch 3 to trigger Interrupt for pin0_29.");

  sjsu::Halt();
  LOG_INFO("Halting any action.");
  return 0;
}

#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Staring GPIO Interrupt Application...\n");

  sjsu::LogInfo(
      "Setting up pin0_15 to interrupt on Rising and Falling edges...");
  sjsu::lpc40xx::Gpio pin0_15(0, 15);
  sjsu::lpc40xx::Gpio pin2_9(2, 9);
  sjsu::lpc40xx::Gpio pin0_30(0, 30);
  sjsu::lpc40xx::Gpio pin0_29(0, 29);

  pin0_15.GetPin().PullUp();
  pin2_9.GetPin().PullUp();
  pin0_30.GetPin().PullUp();
  pin0_29.GetPin().PullUp();

  pin0_15.AttachInterrupt([]() { sjsu::LogInfo("Pin0_15_ISR"); },
                          sjsu::Gpio::Edge::kEdgeBoth);
  pin2_9.AttachInterrupt([]() { sjsu::LogInfo("Pin2_9_ISR"); },
                         sjsu::Gpio::Edge::kEdgeBoth);
  pin0_30.AttachInterrupt([]() { sjsu::LogInfo("Switch 2 has been pressed!"); },
                          sjsu::Gpio::Edge::kEdgeRising);
  pin0_29.AttachInterrupt([]() { sjsu::LogInfo("Switch 3 has been pressed!"); },
                          sjsu::Gpio::Edge::kEdgeFalling);

  sjsu::LogInfo(
      "Setup pin0_29 (Switch 3) to interrupt on only Falling edges...");
  sjsu::LogInfo(
      "Setup pin0_30 (Switch 2) to interrupt on only Rising edges...");
  sjsu::LogInfo("Setup pin0_15 to interrupt on Rising and Falling edges...");
  sjsu::LogInfo("Setup pin2_9 to interrupt on Rising and Falling edges...");

  sjsu::LogInfo(
      "Connect pin0_15 to gnd using a jumper wire to trigger Interrupt.");
  sjsu::LogInfo(
      "Connect pin2_9 to gnd using a jumper wire to trigger Interrupt.");
  sjsu::LogInfo("Press Switch 2 to trigger Interrupt for pin0_30.");
  sjsu::LogInfo("Press Switch 3 to trigger Interrupt for pin0_29.");

  sjsu::LogInfo("Halting any action.");
  sjsu::Halt();
  return 0;
}

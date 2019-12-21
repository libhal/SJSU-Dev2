#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "utility/log.hpp"

int main()
{
  LOG_INFO("Staring GPIO Interrupt Application...\n");

  LOG_INFO("Setting up pin0_15 to interrupt on Rising and Falling edges...");
  sjsu::lpc40xx::Gpio pin0_15(0, 15);
  sjsu::lpc40xx::Gpio pin2_9(2, 9);
  sjsu::lpc40xx::Gpio pin0_30(0, 30);
  sjsu::lpc40xx::Gpio pin0_29(0, 29);

  pin0_15.GetPin().PullUp();
  pin2_9.GetPin().PullUp();
  pin0_30.GetPin().PullUp();
  pin0_29.GetPin().PullUp();

  pin0_15.AttachInterrupt([]() { LOG_INFO("Pin0_15_ISR"); },
                          sjsu::Gpio::Edge::kEdgeBoth);
  pin2_9.AttachInterrupt([]() { LOG_INFO("Pin2_9_ISR"); },
                         sjsu::Gpio::Edge::kEdgeBoth);
  pin0_30.AttachInterrupt([]() { LOG_INFO("Switch 2 has been pressed!"); },
                          sjsu::Gpio::Edge::kEdgeRising);
  pin0_29.AttachInterrupt([]() { LOG_INFO("Switch 3 has been pressed!"); },
                          sjsu::Gpio::Edge::kEdgeFalling);

  LOG_INFO("Setup pin0_29 (Switch 3) to interrupt on only Falling edges...");
  LOG_INFO("Setup pin0_30 (Switch 2) to interrupt on only Rising edges...");
  LOG_INFO("Setup pin0_15 to interrupt on Rising and Falling edges...");
  LOG_INFO("Setup pin2_9 to interrupt on Rising and Falling edges...");

  LOG_INFO("Connect pin0_15 to gnd using a jumper wire to trigger Interrupt.");
  LOG_INFO("Connect pin2_9 to gnd using a jumper wire to trigger Interrupt.");
  LOG_INFO("Press Switch 2 to trigger Interrupt for pin0_30.");
  LOG_INFO("Press Switch 3 to trigger Interrupt for pin0_29.");

  LOG_INFO("Halting any action.");
  sjsu::Halt();
  return 0;
}

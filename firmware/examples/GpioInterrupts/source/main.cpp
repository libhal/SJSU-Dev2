#include <project_config.hpp>

#include <cstdint>
#include <iterator>

#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/gpio.hpp"
#include "L2_HAL/displays/led/onboard_led.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

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

int main(void)
{
  LOG_INFO("Staring GPIO Interrupt Application...\n");

  LOG_INFO("Setting up pin0_15 to interrupt on Rising and Falling edges...");
  Gpio pin0_15(0, 15);
  pin0_15.GetPin().SetMode(Pin::Mode::kPullUp);
  pin0_15.AttachInterrupt(&Port0Pin15ISR, GpioInterface::Edge::kEdgeBoth);
  pin0_15.EnableInterrupts();
  LOG_INFO("Use a jumper wire with gnd and pin0_15 to trigger Interrupt.\n");

  LOG_INFO("Setting up pin2_9 to interrupt on Rising and Falling edges...");
  Gpio pin2_9(2, 9);
  pin2_9.GetPin().SetMode(Pin::Mode::kPullUp);
  pin2_9.AttachInterrupt(&Port2Pin9ISR, GpioInterface::Edge::kEdgeBoth);
  LOG_INFO("Use a jumper wire with gnd and pin2_9 to trigger Interrupt.\n");

  LOG_INFO(
      "Setting up pin0_30 (Switch 2) to interrupt on only Rising edges...");
  Gpio pin0_30(0, 30);
  pin0_30.GetPin().SetMode(Pin::Mode::kPullUp);
  pin0_30.AttachInterrupt(&Port0Pin30ISR, GpioInterface::Edge::kEdgeBoth);
  LOG_INFO("Press Switch 2 to trigger Interrupt for pin0_30.\n");

  LOG_INFO(
      "Setting up pin0_29 (Switch 3) to interrupt on only Falling edges...");
  Gpio pin0_29(0, 29);
  pin0_29.GetPin().SetMode(Pin::Mode::kPullUp);
  pin0_29.AttachInterrupt(&Port0Pin29ISR, GpioInterface::Edge::kEdgeBoth);
  LOG_INFO("Press Switch 3 to trigger Interrupt for pin0_29.");

  Halt();
  LOG_INFO("Halting any action.");
  return 0;
}

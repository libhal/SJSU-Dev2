#include <cstdio>

#include "L0_Platform/startup.hpp"
#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "L1_Peripheral/cortex/system_timer.hpp"
#include "L1_Peripheral/msp432p401r/gpio.hpp"
#include "L1_Peripheral/msp432p401r/pin.hpp"
#include "L1_Peripheral/msp432p401r/system_controller.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

namespace
{
void ConfigureSystemClocks()
{
  auto & system_controller    = sjsu::SystemController::GetPlatformController();
  auto & clock_configuration = system_controller.GetClockConfiguration<
      sjsu::msp432p401r::SystemController::ClockConfiguration_t>();
  clock_configuration.dco.frequency = 12_MHz;
  // Set HSMCLK P4.4 to output ~3 MHz
  clock_configuration.subsystem_master.divider =
      sjsu::msp432p401r::SystemController::ClockDivider::kDivideBy4;
  // Set SMCLK P7.0 to output ~1.5 MHz
  clock_configuration.subsystem_master.low_speed_divider =
      sjsu::msp432p401r::SystemController::ClockDivider::kDivideBy8;

  sjsu::InitializePlatform();
}

/// Configures P4.2, P4.3, P4.4, and P7.0 to output ACLK, MCLK, HSMCLK, and
/// SMCLK respectively. This allows the use of an oscilloscope to examine each
/// of the clock signals through the corresponding pin.
void ConfigureClockOutPins()
{
  constexpr uint8_t kClockOutFunction = 0b101;

  // Configure P4.2 to output ACLK
  sjsu::msp432p401r::Pin aclk_out_pin(4, 2);
  aclk_out_pin.ConfigureFunction(kClockOutFunction);

  // Configure P4.3 to output MCLK
  sjsu::msp432p401r::Pin mclk_out_pin(4, 3);
  mclk_out_pin.ConfigureFunction(kClockOutFunction);

  // Configure P4.4 to output HSMCLK
  sjsu::msp432p401r::Pin hsmclk_out_pin(4, 4);
  hsmclk_out_pin.ConfigureFunction(kClockOutFunction);

  // Configure P7.0 to output SMCLK
  sjsu::msp432p401r::Pin smclk_out_pin(7, 0);
  smclk_out_pin.ConfigureFunction(kClockOutFunction);
}
}  // namespace

int main()
{
  sjsu::LogInfo("Starting MSP432P401R System Controller Demo...");

  // Configure the on-board P1.0 LED to be initially turned on.
  sjsu::msp432p401r::Gpio p1_0(1, 0);
  p1_0.SetAsOutput();
  p1_0.SetHigh();

  ConfigureSystemClocks();
  ConfigureClockOutPins();

  while (true)
  {
    p1_0.Toggle();
    sjsu::Delay(1s);
  }

  return 0;
}

#include <cstdio>

#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "L1_Peripheral/cortex/system_timer.hpp"
#include "L1_Peripheral/msp432p401r/gpio.hpp"
#include "L1_Peripheral/msp432p401r/pin.hpp"
#include "L1_Peripheral/msp432p401r/system_controller.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

namespace
{
/// Configures P4.2, P4.3, P4.4, and P7.0 to output ACLK, MCLK, HSMCLK, and
/// SMCLK respectively. This allows the use of an oscilloscope to examine each
/// of the clock signals through the corresponding pin.
void ConfigureClockOutPins()
{
  constexpr uint8_t kClockOutFunction = 0b101;

  // Configure P4.2 to output ACLK
  sjsu::msp432p401r::Pin aclk_out_pin(4, 2);
  aclk_out_pin.SetPinFunction(kClockOutFunction);

  // Configure P4.3 to output MCLK
  sjsu::msp432p401r::Pin mclk_out_pin(4, 3);
  mclk_out_pin.SetPinFunction(kClockOutFunction);

  // Configure P4.4 to output HSMCLK
  sjsu::msp432p401r::Pin hsmclk_out_pin(4, 4);
  hsmclk_out_pin.SetPinFunction(kClockOutFunction);

  // Configure P7.0 to output SMCLK
  sjsu::msp432p401r::Pin smclk_out_pin(7, 0);
  smclk_out_pin.SetPinFunction(kClockOutFunction);
}
}  // namespace

int main()
{
  LOG_INFO("Starting MSP432P401R System Controller Demo...");

  // Configure the on-board P1.0 LED to be initially turned on.
  sjsu::msp432p401r::Gpio p1_0(1, 0);
  p1_0.SetAsOutput();
  p1_0.SetHigh();

  sjsu::msp432p401r::SystemController system_controller;
  // MCLK P4.3 should output ~12MHz
  // HSMCLK P4.4 should output ~3MHz
  // SMCLK P7.0 should output ~1.5MHz
  system_controller.SetSystemClockFrequency(12_MHz);
  system_controller.SetPeripheralClockDivider({}, 4);
  system_controller.SetClockDivider(
      sjsu::msp432p401r::SystemController::Clock::kLowSpeedSubsystemMaster, 8);

  constexpr auto kDummySystemTimerPeripheralID =
      sjsu::SystemController::PeripheralID::Define<0>();
  sjsu::cortex::SystemTimer system_timer(kDummySystemTimerPeripheralID);
  system_timer.SetTickFrequency(config::kRtosFrequency);

  ConfigureClockOutPins();

  while (true)
  {
    p1_0.Toggle();
    sjsu::Delay(1s);
  }

  return 0;
}

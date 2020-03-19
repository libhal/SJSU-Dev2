#include <cstdio>

#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "L1_Peripheral/cortex/system_timer.hpp"
#include "L1_Peripheral/msp432p401r/system_controller.hpp"
#include "utility/bit.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

namespace
{
/// Configures P4.2, P4.3, P4.4, and P7.0 to output ACLK, MCLK, HSMCLK, and
/// SMCLK respectively. This allows the use of an oscilloscope to examine each
/// of the clock signals through the corresponding pin.
void ConfigureClockOutPins()
{
  // Configure P4.2 to output ACLK
  constexpr sjsu::bit::Mask kAclkBitMask = sjsu::bit::CreateMaskFromRange(2);
  sjsu::msp432p401r::P4->SEL1 =
      sjsu::bit::Clear(sjsu::msp432p401r::P4->SEL1, kAclkBitMask);
  sjsu::msp432p401r::P4->SEL0 =
      sjsu::bit::Set(sjsu::msp432p401r::P4->SEL0, kAclkBitMask);
  sjsu::msp432p401r::P4->DIR =
      sjsu::bit::Set(sjsu::msp432p401r::P4->DIR, kAclkBitMask);

  // Configure P4.3 to output MCLK
  constexpr sjsu::bit::Mask kMclkBitMask = sjsu::bit::CreateMaskFromRange(3);
  sjsu::msp432p401r::P4->SEL1 =
      sjsu::bit::Clear(sjsu::msp432p401r::P4->SEL1, kMclkBitMask);
  sjsu::msp432p401r::P4->SEL0 =
      sjsu::bit::Set(sjsu::msp432p401r::P4->SEL0, kMclkBitMask);
  sjsu::msp432p401r::P4->DIR =
      sjsu::bit::Set(sjsu::msp432p401r::P4->DIR, kMclkBitMask);

  // Configure P4.4 to output HSMCLK
  constexpr sjsu::bit::Mask kHsmclkBitMask = sjsu::bit::CreateMaskFromRange(4);
  sjsu::msp432p401r::P4->SEL1 =
      sjsu::bit::Clear(sjsu::msp432p401r::P4->SEL1, kHsmclkBitMask);
  sjsu::msp432p401r::P4->SEL0 =
      sjsu::bit::Set(sjsu::msp432p401r::P4->SEL0, kHsmclkBitMask);
  sjsu::msp432p401r::P4->DIR =
      sjsu::bit::Set(sjsu::msp432p401r::P4->DIR, kHsmclkBitMask);

  // Configure P7.0 to output SMCLK
  constexpr sjsu::bit::Mask kSmclkBitMask = sjsu::bit::CreateMaskFromRange(0);
  sjsu::msp432p401r::P7->SEL1 =
      sjsu::bit::Clear(sjsu::msp432p401r::P7->SEL1, kSmclkBitMask);
  sjsu::msp432p401r::P7->SEL0 =
      sjsu::bit::Set(sjsu::msp432p401r::P7->SEL0, kSmclkBitMask);
  sjsu::msp432p401r::P7->DIR =
      sjsu::bit::Set(sjsu::msp432p401r::P7->DIR, kSmclkBitMask);
}
}  // namespace

int main()
{
  LOG_INFO("Starting MSP432P401R System Controller Demo...");
  // Configure the on-board P1.0 LED to be initially turned on.
  // TODO(#869): Should initialize the pin using the GPIO class.
  constexpr sjsu::bit::Mask kLedBitMask = sjsu::bit::CreateMaskFromRange(0);
  sjsu::msp432p401r::P1->DIR            = sjsu::bit::Set(
      sjsu::msp432p401r::P1->DIR, sjsu::bit::CreateMaskFromRange(0));
  sjsu::msp432p401r::P1->OUT =
      sjsu::bit::Set(sjsu::msp432p401r::P1->OUT, kLedBitMask);

  sjsu::msp432p401r::SystemController system_controller;
  // MCLK P4.3 should output ~12MHz
  // HSMCLK P4.4 should output ~3MHz
  // SMCLK P7.0 should output ~1.5MHz
  system_controller.SetSystemClockFrequency(12_MHz);
  system_controller.SetPeripheralClockDivider({}, 4);
  system_controller.SetClockDivider(
      sjsu::msp432p401r::SystemController::Clock::kLowSpeedSubsystemMaster, 8);

  sjsu::cortex::SystemTimer system_timer;
  system_timer.SetTickFrequency(config::kRtosFrequency);

  ConfigureClockOutPins();

  while (true)
  {
    // TODO(#869): Should use the Toggle() function from GPIO class.
    sjsu::msp432p401r::P1->OUT =
        sjsu::bit::Toggle(sjsu::msp432p401r::P1->OUT, kLedBitMask);
    sjsu::Delay(1s);
  }

  return 0;
}

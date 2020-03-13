#include <cstdio>

#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "utility/bit.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Starting MSP432P401R LED Blinker Demo...");
  // Configure the on-board P1.0 LED to be initially turned on.
  constexpr sjsu::bit::Mask kLedBitMask = sjsu::bit::CreateMaskFromRange(0);
  sjsu::msp432p401r::P1->DIR            = sjsu::bit::Set(
      sjsu::msp432p401r::P1->DIR, sjsu::bit::CreateMaskFromRange(0));
  sjsu::msp432p401r::P1->OUT =
      sjsu::bit::Set(sjsu::msp432p401r::P1->OUT, kLedBitMask);

  while (true)
  {
    sjsu::msp432p401r::P1->OUT =
        sjsu::bit::Toggle(sjsu::msp432p401r::P1->OUT, kLedBitMask);
    sjsu::Delay(1s);
  }
  return 0;
}

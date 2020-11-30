#include <cstdio>

#include "L0_Platform/msp432p401r/msp432p401r.h"
#include "L1_Peripheral/msp432p401r/gpio.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Starting MSP432P401R LED Blinker Demo...");

  // Configure the on-board P1.0 LED to be initially turned on.
  sjsu::msp432p401r::Gpio p1_0(1, 0);
  p1_0.Initialize();
  p1_0.Enable();
  p1_0.SetAsOutput();
  p1_0.SetHigh();

  while (true)
  {
    p1_0.Toggle();
    sjsu::Delay(1s);
  }
  return 0;
}

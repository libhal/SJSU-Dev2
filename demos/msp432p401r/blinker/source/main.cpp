#include <cstdio>

#include "platforms/targets/msp432p401r/msp432p401r.h"
#include "peripherals/msp432p401r/gpio.hpp"
#include "utility/log.hpp"
#include "utility/time/time.hpp"

int main()
{
  sjsu::LogInfo("Starting MSP432P401R LED Blinker Demo...");

  // Configure the on-board P1.0 LED to be initially turned on.
  sjsu::msp432p401r::Gpio & p1_0 = sjsu::msp432p401r::GetGpio<1, 0>();
  p1_0.Initialize();
  p1_0.SetAsOutput();
  p1_0.SetHigh();

  while (true)
  {
    p1_0.Toggle();
    sjsu::Delay(1s);
  }
  return 0;
}

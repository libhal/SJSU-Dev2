#include <cstdint>

#include "L0_LowLevel/LPC40xx.h"
#include "L0_LowLevel/startup.hpp"
#include "L1_Drivers/pin.hpp"
#include "L2_Utilities/debug_print.hpp"

constexpr uint8_t kGpioFunction = 0b000;
Pin led3(Pin::CreatePin<1, 15>());

void DemoSystemIsr()
{
  // Simply toggle the LED off and on
  LPC_GPIO1->PIN ^= (1 << 15);
}

// There is another LowLevelInit() function in the startup.cpp file. That
// LowLevelInit is declared as __attribute__((weak)), which means that it can be
// overridden by another declaration of a function with the same signature.
// Signature meaning the same return type, name and parameters.
// When ever you override global function you should put "// override" above it
//
// Overrides "void LowLevelInit()" in startup.cpp
void LowLevelInit()
{
  led3.SetPinFunction(kGpioFunction);
  // Set Pin 15 as output
  LPC_GPIO1->DIR |= (1 << 15);
  // Default pin state to LOW (turns on LED)
  LPC_GPIO1->PIN &= ~(1 << 15);
  system_timer.SetIsrFunction(DemoSystemIsr);
  // Frequency is in Hz
  system_timer.SetTickFrequency(50);
  bool timer_started_successfully = system_timer.StartTimer();
  if (timer_started_successfully)
  {
    DEBUG_PRINT("Demo System Timer has begun.");
  }
  else
  {
    DEBUG_PRINT("Demo System Timer has FAILED!!");
  }
}

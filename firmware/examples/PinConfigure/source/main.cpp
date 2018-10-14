#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "config.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin.hpp"
#include "L2_Utilities/debug_print.hpp"

int main(void)
{
  // This application assumes that all pins are set to function 0 (GPIO)
  DEBUG_PRINT("Pin Configure Application Starting...");
  // Using constructor directly to constuct Pin object
  // This is discouraged, since this constructor does not perform any compile
  // time checks on the port or pin value
  Pin p0_0(0, 7);
  p0_0.SetMode(PinInterface::Mode::kInactive);
  DEBUG_PRINT("Disabling both pull up and down resistors for P0.0...");
  // Prefered option of constructing Pin, since this factory call is
  // done in compile time and will perform compile time validation on the port
  // and pin template parameters.
  Pin p1_24 = Pin::CreatePin<1, 24>();
  p1_24.SetMode(PinInterface::Mode::kPullDown);
  DEBUG_PRINT("Enabling P1.24 pull down resistor...");

  Pin p2_0 = Pin::CreatePin<2, 0>();
  p2_0.SetMode(PinInterface::Mode::kPullUp);
  DEBUG_PRINT("Enabling P2.0 pull up resistor...");

  Pin p4_28 = Pin::CreatePin<4, 29>();
  p4_28.SetMode(PinInterface::Mode::kRepeater);
  DEBUG_PRINT("Setting P4.29 to repeater mode...");

  DEBUG_PRINT(
      "Use some jumpers and a multimeter to test each pin to see if the "
      "internal pull up and pull down resistors are working.");
  DEBUG_PRINT(
      "Use a jumper and resistor to pull P4.28 high or low. If you check the "
      "pin with a multimeter you will see that the pin retains the previous "
      "input value.");

  DEBUG_PRINT("Halting any action.");
  while (1)
  {
    continue;
  }
  return 0;
}

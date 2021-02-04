#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "peripherals/lpc40xx/pin.hpp"
#include "utility/log.hpp"

int main()
{
  // This application assumes that all pins are set to function 0 (GPIO)
  sjsu::LogInfo("Pin Configure Application Starting...");

  // Set P0[7] to floating (LPC40xx defaults pins to pull up)
  sjsu::lpc40xx::Pin & p0_0 = sjsu::lpc40xx::GetPin<0, 7>();
  p0_0.settings.Floating();
  p0_0.Initialize();

  sjsu::LogInfo("Disabling both pull up and down resistors for P0.0...");
  sjsu::lpc40xx::Pin & p1_24 = sjsu::lpc40xx::GetPin<1, 24>();
  p1_24.settings.PullDown();
  p1_24.Initialize();
  sjsu::LogInfo("Enabling P1.24 pull down resistor...");

  sjsu::lpc40xx::Pin & p2_0 = sjsu::lpc40xx::GetPin<2, 0>();
  p2_0.settings.PullUp();
  p2_0.Initialize();
  sjsu::LogInfo("Enabling P2.0 pull up resistor...");

  sjsu::LogInfo(
      "Use some jumpers and a multimeter to test each pin to see if the "
      "internal pull up and pull down resistors are working.");

  sjsu::LogInfo(
      "Use a jumper and resistor to pull P4.28 high or low. If you check the "
      "pin with a multimeter you will see that the pin retains the previous "
      "input value. This capability is called repeater mode.");

  return 0;
}

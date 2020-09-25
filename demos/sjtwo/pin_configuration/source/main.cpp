#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "utility/log.hpp"

int main()
{
  // This application assumes that all pins are set to function 0 (GPIO)
  sjsu::LogInfo("Pin Configure Application Starting...");

  // Set P0[7] to floating (LPC40xx defaults pins to pull up)
  sjsu::lpc40xx::Pin p0_0(0, 7);
  p0_0.Initialize();
  p0_0.Enable();
  p0_0.ConfigureFloating();

  sjsu::LogInfo("Disabling both pull up and down resistors for P0.0...");
  sjsu::lpc40xx::Pin p1_24(1, 24);
  p1_24.ConfigurePullDown();
  p1_24.Initialize();
  p1_24.Enable();
  sjsu::LogInfo("Enabling P1.24 pull down resistor...");

  sjsu::lpc40xx::Pin p2_0(2, 0);
  p2_0.Initialize();
  p2_0.Enable();
  p2_0.ConfigurePullUp();
  sjsu::LogInfo("Enabling P2.0 pull up resistor...");

  sjsu::lpc40xx::Pin p4_28(4, 29);
  p4_28.Initialize();
  p4_28.Enable();
  p4_28.ConfigurePullResistor(sjsu::Pin::Resistor::kRepeater);
  sjsu::LogInfo("Setting P4.29 to repeater mode...");

  sjsu::LogInfo(
      "Use some jumpers and a multimeter to test each pin to see if the "
      "internal pull up and pull down resistors are working.");

  sjsu::LogInfo(
      "Use a jumper and resistor to pull P4.28 high or low. If you check the "
      "pin with a multimeter you will see that the pin retains the previous "
      "input value. This capability is called repeater mode.");

  return 0;
}

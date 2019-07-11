#include <cinttypes>
#include <cstdint>
#include <cstdio>

#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::lpc40xx::SystemController clock;
  sjsu::lpc40xx::Pin clock_pin(1, 25);

  clock_pin.SetPinFunction(0b101);  //  set clock to putput mode
  clock_pin.SetPull(sjsu::Pin::Resistor::kNone);
  clock_pin.EnableHysteresis(false);
  clock_pin.SetAsActiveLow(false);
  clock_pin.EnableFastMode(false);
  clock_pin.SetAsOpenDrain(false);

  using sjsu::lpc40xx::LPC_SC_TypeDef;
  LPC_SC->CLKOUTCFG |= (1 << 8);
  LPC_SC->CLKOUTCFG &= ~(0xFF << 0);

  while (true)
  {
    uint32_t speed;
    clock.SetSystemClockFrequency(12);
    sjsu::Delay(5000);
    clock.SetSystemClockFrequency(48);
    sjsu::Delay(5000);
    speed = clock.GetSystemFrequency();
    LOG_INFO("Speed is %" PRIu32, speed);
  }
  return 0;
}

#include <project_config.hpp>
#include <cinttypes>
#include <cstdint>
#include <cstdio>

#include "L1_Drivers/pin.hpp"
#include "L1_Drivers/system_clock.hpp"
#include "L2_Utilities/debug_print.hpp"

int main(void)
{
  SystemClock clock;
  Pin clock_pin(1, 25);
  clock_pin.SetPinFunction(0b101);  //  set clock to putput mode
  clock_pin.SetMode(PinInterface::Mode::kInactive);
  clock_pin.EnableHysteresis(false);
  clock_pin.SetAsActiveLow(false);
  clock_pin.EnableFastMode(false);
  clock_pin.SetAsOpenDrain(false);

  LPC_SC->CLKOUTCFG |= (1 << 8);
  LPC_SC->CLKOUTCFG &= ~(0xFF << 0);

  while (true)
  {
    uint32_t speed;
    clock.SetClockFrequency(12);
    Delay(5000);
    clock.SetClockFrequency(48);
    Delay(5000);
    speed = clock.GetClockFrequency();
    DEBUG_PRINT("Speed is %" PRIu32, speed);
  }
  return 0;
}

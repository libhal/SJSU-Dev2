#include <cinttypes>
#include <cstdint>
#include <cstdio>

#include "L0_LowLevel/system_controller.hpp"
#include "L1_Drivers/pin.hpp"
#include "utility/log.hpp"

int main(void)
{
  Lpc40xxSystemController clock;
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
    speed = clock.GetSystemFrequency();
    LOG_INFO("Speed is %" PRIu32, speed);
  }
  return 0;
}

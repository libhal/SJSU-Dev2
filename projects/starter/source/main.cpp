#include <cstdio>
#include <functional>
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/hardware_counter.hpp"

int main()
{
  sjsu::lpc40xx::Gpio gpio(0, 0);
  sjsu::GpioCounter counter(gpio, sjsu::Gpio::Edge::kEdgeBoth);

  counter.Initialize();

  uint32_t previous_count = 0;
  while (true)
  {
    uint32_t current_count = counter.GetCount();
    if (previous_count != current_count)
    {
      LOG_INFO("Hardare Count %" PRIu32 "\n", current_count);
      previous_count = current_count;
    }
  }

  return 0;
}
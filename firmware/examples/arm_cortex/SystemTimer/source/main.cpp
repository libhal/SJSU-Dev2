#include "L1_Peripheral/cortex/system_timer.hpp"
#include "utility/log.hpp"

void DemoSystemIsr()
{
  static int counter = 0;
  LOG_INFO("System Timer Tick #%d ...", counter++);
}

int main()
{
  LOG_INFO("System Timer Application Starting...");

  sjsu::cortex::SystemTimer system_timer;
  system_timer.SetInterrupt(DemoSystemIsr);
  // Frequency is in Hz
  system_timer.SetTickFrequency(10);
  system_timer.StartTimer();

  LOG_INFO("Halting any action.");
  return 0;
}

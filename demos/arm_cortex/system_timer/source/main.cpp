#include "L1_Peripheral/cortex/system_timer.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/system_controller.hpp"
#include "utility/log.hpp"

void DemoSystemIsr()
{
  static int counter = 0;
  LOG_INFO("System Timer Tick #%d ...", counter++);
}

int main()
{
  LOG_INFO("System Timer Application Starting...");
  sjsu::cortex::SystemTimer system_timer(
      sjsu::GetInactive<sjsu::SystemController>());

  system_timer.SetInterrupt(DemoSystemIsr);
  system_timer.SetTickFrequency(10_Hz);
  system_timer.StartTimer();

  LOG_INFO("Halting any action.");
  return 0;
}

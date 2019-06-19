#include "L1_Peripheral/cortex/system_timer.hpp"
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
  sjsu::DefaultSystemController default_system_controller;
  sjsu::cortex::SystemTimer system_timer(default_system_controller);

  system_timer.SetInterrupt(DemoSystemIsr);
  system_timer.SetTickFrequency(10 /* Hz */);
  system_timer.StartTimer();

  LOG_INFO("Halting any action.");
  return 0;
}

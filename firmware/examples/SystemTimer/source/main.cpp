#include "L1_Drivers/gpio.hpp"
#include "L1_Drivers/system_timer.hpp"
#include "utility/log.hpp"

Gpio led3(1, 18);

void DemoSystemIsr()
{
  // Simply toggle the LED off and on
  led3.Toggle();
}

int main(void)
{
  LOG_INFO("System Timer Application Starting...");

  SystemTimer system_timer;
  // Set Pin 18 as output
  led3.SetAsOutput();
  // Default pin state to LOW (turns on LED)
  led3.SetHigh();
  system_timer.SetIsrFunction(DemoSystemIsr);
  // Frequency is in Hz
  system_timer.SetTickFrequency(10);
  system_timer.StartTimer();

  LOG_INFO("Halting any action.");
  return 0;
}

#include "L1_Drivers/gpio.hpp"
#include "L2_HAL/displays/led/onboard_led.hpp"
#include "L3_Application/periodic_scheduler.hpp"
#include "utility/log.hpp"

OnBoardLed leds;
/// Toggles the led every 1s
void BlinkLed1Hz(uint32_t)
{
  leds.Toggle(0);
}
/// Toggles the led every 0.1s
void BlinkLed10Hz(uint32_t)
{
  leds.Toggle(1);
}

rtos::PeriodicScheduler scheduler = rtos::PeriodicScheduler();
rtos::PeriodicTask<512> blinker_1_hz_task("BlinkLed1Hz", rtos::Priority::kLow,
                                          BlinkLed1Hz);
rtos::PeriodicTask<512> blinker_10_hz_task("BlinkLed10Hz", rtos::Priority::kLow,
                                           BlinkLed10Hz);

int main()
{
  LOG_INFO("Starting PeriodicScheduler example...");
  // set initial state of the leds
  leds.Initialize();
  leds.On(0);
  leds.On(1);
  scheduler.SetTask(&blinker_1_hz_task,
                    rtos::PeriodicScheduler::Frequency::k1Hz);
  scheduler.SetTask(&blinker_10_hz_task,
                    rtos::PeriodicScheduler::Frequency::k10Hz);
  rtos::TaskScheduler::Instance().Start();
  return 0;
}

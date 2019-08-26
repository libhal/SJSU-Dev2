#include <cstdint>
#include <iterator>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/watchdog.hpp"
#include "L3_Application/task.hpp"
#include "third_party/units/units.h"
#include "utility/time.hpp"
#include "utility/rtos.hpp"
#include "utility/log.hpp"

sjsu::lpc40xx::Watchdog watchdog;

sjsu::lpc40xx::Gpio button3(0, 29);
sjsu::lpc40xx::Gpio led3(1, 18);

void FeedTask([[maybe_unused]] void * pv_paramater)
{
  while (true)
  {
    watchdog.FeedSequence();
  }
}

void HighCPUTask([[maybe_unused]] void * pv_paramater)
{
  while (true)
  {
    // While inside this task, LED3 turns on.
    led3.SetLow();

    // While button 3 (SW3) is pressed, a delay will go beyond the time of
    // the watchdog and trigger the watchdog interrupt.
    if (button3.Read())
    {
      // Because the FeedTask() does not get a chance to run during the delay
      // the watchdog is starved, thus causing a reset.
      // When the watchdog interrupt is triggered, LED0 flashes, indicating
      // that the watchdog is about to cause a reset.
      // vTaskDelay is not used because it will block this task and execute
      // FeedTask() in the meantime.
      sjsu::Delay(20s);
    }
  }
}

int main()
{
  button3.SetAsInput();
  led3.SetDirection(sjsu::Gpio::Direction::kOutput);

  // The parameter for this function is in seconds.
  // The first parameter is the value stored in the watchdog counter.
  watchdog.Initialize(10s);
  watchdog.RegisterWatchdogHandler();

  // The watchdog is fed for the first time to start the watchdog timer.
  watchdog.FeedSequence();

  xTaskCreate(FeedTask, "FeedTask", 1024, NULL, sjsu::rtos::kLow, NULL);
  xTaskCreate(HighCPUTask, "HighCPUTask", 1024, NULL, sjsu::rtos::kHigh, NULL);
  vTaskStartScheduler();

  sjsu::Halt();
}

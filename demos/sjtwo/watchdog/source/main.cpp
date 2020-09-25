#include <cstdint>
#include <iterator>

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L0_Platform/ram.hpp"
#include "L1_Peripheral/interrupt.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/watchdog.hpp"
#include "L3_Application/task_scheduler.hpp"
#include "third_party/units/units.h"
#include "utility/log.hpp"
#include "utility/rtos.hpp"
#include "utility/time.hpp"

sjsu::lpc40xx::Watchdog watchdog;

sjsu::lpc40xx::Gpio button3(0, 29);
sjsu::lpc40xx::Gpio led3(1, 18);

void FeedTask([[maybe_unused]] void * param)
{
  while (true)
  {
    watchdog.FeedSequence();
  }
}

void HighCPUTask([[maybe_unused]] void * param)
{
  sjsu::LogInfo("Press button 3 to have this task hog the CPU for 2 seconds.");
  sjsu::LogInfo("Doing so will cause the watchdog to reset the system.");
  while (true)
  {
    // While inside this task, LED3 turns on.
    led3.SetHigh();
    // While button 3 (SW3) is pressed, a delay will go beyond the time of
    // the watchdog and trigger the watchdog interrupt.
    if (button3.Read())
    {
      sjsu::LogInfo("High priority task is now hogging the CPU for 2 second.");
      sjsu::LogInfo("System will restart!\n\n");
      led3.SetLow();
      // Because the LowPriorityFeedTask() does not get a chance to run during
      // the delay the watchdog is starved, thus causing a reset. When the
      // watchdog interrupt is triggered, LED0 flashes, indicating that the
      // watchdog is about to cause a reset. vTaskDelay is not used because it
      // will block this task and execute LowPriorityFeedTask() in the meantime.
      sjsu::Delay(2s);
    }
    vTaskDelay(10);
  }
}

int main()
{
  sjsu::LogInfo("Watchdog Timer Application Starting...");

  button3.Initialize();
  led3.Initialize();

  button3.Enable();
  led3.Enable();

  button3.SetAsInput();
  led3.SetDirection(sjsu::Gpio::Direction::kOutput);

  // The parameter for this function is in seconds.
  // The first parameter is the value stored in the watchdog counter.
  watchdog.Initialize(1s);
  watchdog.Enable();

  // The watchdog is fed for the first time to start the watchdog timer.
  watchdog.FeedSequence();

  xTaskCreate(FeedTask, "FeedTask", 1024, NULL, sjsu::rtos::kLow, NULL);
  xTaskCreate(HighCPUTask, "HighCPUTask", 1024, NULL, sjsu::rtos::kHigh, NULL);

  vTaskStartScheduler();

  sjsu::Halt();
}

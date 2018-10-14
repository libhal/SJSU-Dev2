#include <FreeRTOS.h>
#include <task.h>

#include "L2_Utilities/macros.hpp"

volatile uint64_t milliseconds = 0;

uint64_t Milliseconds()
{
  return milliseconds;
}

void Delay(uint32_t delay_time)
{
#if defined HOST_TEST
  SJ2_USED(delay_time);
  return;
#else
  if (taskSCHEDULER_RUNNING == xTaskGetSchedulerState())
  {
    vTaskDelay(delay_time);
  }
  else
  {
    uint64_t time_after_delay = milliseconds + delay_time;
    while (milliseconds < time_after_delay)
    {
      continue;
    }
  }
#endif  // HOST_TEST
}

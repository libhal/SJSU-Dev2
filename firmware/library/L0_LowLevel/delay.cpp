#include <FreeRTOS.h>
#include <task.h>

volatile uint64_t milliseconds;

uint64_t Milliseconds()
{
    return milliseconds;
}

void Delay(uint32_t delay_time)
{
#if defined HOST_TEST
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

#include "L3_Application/periodic_scheduler.hpp"
#include "utility/log.hpp"
/// Task function to prints a message every 1s
sjsu::rtos::PeriodicTaskInterface::TaskFunction task_function_1_hz =
    [](uint32_t run_count) { LOG_INFO("1 Hz counter: %lu", run_count); };
/// Task function to prints a message every 100ms
sjsu::rtos::PeriodicTaskInterface::TaskFunction task_function_10_hz =
    [](uint32_t run_count) { LOG_INFO("10 Hz counter: %lu", run_count); };

sjsu::rtos::TaskScheduler scheduler;
sjsu::rtos::PeriodicScheduler periodic_scheduler("PeriodicScheduler",
                                                 scheduler);
sjsu::rtos::PeriodicTask<512> printer_1_hz_task("Print1Hz",
                                                sjsu::rtos::Priority::kLow,
                                                &task_function_1_hz,
                                                scheduler);
sjsu::rtos::PeriodicTask<512> printer_10_hz_task("Print10Hz",
                                                 sjsu::rtos::Priority::kLow,
                                                 &task_function_10_hz,
                                                 scheduler);

int main()
{
  LOG_INFO("Starting PeriodicScheduler example...");
  periodic_scheduler.SetTask(&printer_1_hz_task,
                             sjsu::rtos::PeriodicScheduler::Frequency::k1Hz);
  periodic_scheduler.SetTask(&printer_10_hz_task,
                             sjsu::rtos::PeriodicScheduler::Frequency::k10Hz);
  scheduler.Start();
  return 0;
}

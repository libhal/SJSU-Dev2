#include "L3_Application/periodic_scheduler.hpp"
#include "utility/log.hpp"

namespace  // private namespace
{
/// Task function to prints a message every 1s
sjsu::rtos::PeriodicTaskInterface::TaskFunction task_function_1_hz =
    [](uint32_t run_count) { sjsu::LogInfo("1 Hz counter: %lu", run_count); };
/// Task function to prints a message every 100ms
sjsu::rtos::PeriodicTaskInterface::TaskFunction task_function_10_hz =
    [](uint32_t run_count) { sjsu::LogInfo("10 Hz counter: %lu", run_count); };

sjsu::rtos::TaskScheduler scheduler;
sjsu::rtos::PeriodicScheduler periodic_scheduler("PeriodicScheduler");
sjsu::rtos::PeriodicTask<1024> printer_1_hz_task("Print1Hz",
                                                 sjsu::rtos::Priority::kLow,
                                                 &task_function_1_hz);
sjsu::rtos::PeriodicTask<1024> printer_10_hz_task("Print10Hz",
                                                  sjsu::rtos::Priority::kLow,
                                                  &task_function_10_hz);
}  // namespace

int main()
{
  sjsu::LogInfo("Starting PeriodicScheduler example...");

  scheduler.AddTask(&printer_1_hz_task);
  scheduler.AddTask(&printer_10_hz_task);

  periodic_scheduler.SetTask(&printer_1_hz_task,
                             sjsu::rtos::PeriodicScheduler::Frequency::kF1Hz);
  periodic_scheduler.SetTask(&printer_10_hz_task,
                             sjsu::rtos::PeriodicScheduler::Frequency::kF10Hz);

  scheduler.Start();

  return 0;
}

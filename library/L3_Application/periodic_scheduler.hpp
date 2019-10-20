
// This file contains the PeriodicTask and PeriodicScheduler class that allows
// task functions to be scheduled and executed at fixed frequencies of 1Hz,
// 10Hz, 100Hz, and 1000Hz.
//
// NOTE: Both the PeriodicTask and PeriodicScheduler inherits from the Task
//       class and must be persistent or in global space.
//
// Usage:
//    void TaskFunction10Hz(uint32_t count)
//    {
//      // do something...
//    }
//    rtos::PeriodicScheduler scheduler = rtos::PeriodicScheduler<512>();
//    rtos::PeriodicTask<512> task_10Hz("Task10Hz",
//                                      rtos::Priority::kLow,
//                                      TaskFunction10Hz);
//    scheduler.SetTask(&task_10Hz, rtos::PeriodicScheduler::Frequency::k10Hz);
//    rtos::TaskScheduler::Instance().Start();
#pragma once

#include "L3_Application/task.hpp"

#include "semphr.h"
#include "timers.h"

#include "utility/enum.hpp"
#include "utility/log.hpp"

namespace sjsu
{
namespace rtos
{
/// Pointer to the function containing the code to be executed periodically.
/// @note The function should not contain any blocking code. eg. vTaskDelay()
///       and vTaskDelayUntil() should not be used within the function.
///
/// @param count Number of times the task function has been run.
using PeriodicTaskFunction = void (*)(uint32_t count);

class PeriodicTaskInterface
{
 public:
  virtual PeriodicTaskFunction GetTaskFunction() const = 0;
  /// @return Returns the periodic semaphore that is used by the
  ///         PeriodicScheduler to manage the PeriodicTask
  virtual SemaphoreHandle_t GetPeriodicSemaphore()     = 0;
  /// @return Returns the number of times the task has been executed.
  virtual uint32_t GetRunCount()                       = 0;
};
/// @tparam kTaskStackSize Task stack size in bytes.
template <size_t kTaskStackSize>
class PeriodicTask : public Task<kTaskStackSize>,
                     public virtual PeriodicTaskInterface
{
 public:
  PeriodicTask(const char * name,
               Priority priority,
               PeriodicTaskFunction task_function)
      : Task<kTaskStackSize>(name, priority), task_function_(task_function)
  {
    SJ2_ASSERT_FATAL(task_function_ != nullptr,
                     "Task function cannot be null for task: %s", name);
    semaphore_ = xSemaphoreCreateBinaryStatic(&semaphore_buffer_);
    SJ2_ASSERT_FATAL(semaphore_ != nullptr,
                     "Error creating periodic semaphore for task: %s", name);
    run_count_ = 0;
  }
  bool Run() override
  {
    while (xSemaphoreTake(semaphore_, portMAX_DELAY))
    {
      task_function_(++run_count_);
    }
    return true;
  }
  PeriodicTaskFunction GetTaskFunction() const override
  {
    return task_function_;
  }
  SemaphoreHandle_t GetPeriodicSemaphore() override
  {
    return semaphore_;
  }
  uint32_t GetRunCount() override
  {
    return run_count_;
  }

 protected:
  PeriodicTaskFunction task_function_;
  StaticSemaphore_t semaphore_buffer_;
  /// A semaphore managed by the PeriodicScheduler to control when the task is
  /// executed.
  SemaphoreHandle_t semaphore_;
  /// Number of times the task function has been executed.
  uint32_t run_count_;
};

class PeriodicScheduler final : public Task<512>
{
 public:
  enum class Frequency : uint8_t
  {
    k1Hz = 0,  // NOLINT(readability-identifier-naming)
    k10Hz,     // NOLINT(readability-identifier-naming)
    k100Hz,    // NOLINT(readability-identifier-naming)
    k1000Hz,   // NOLINT(readability-identifier-naming)
    kCount
  };

  static constexpr size_t kMaxTaskCount = Value(Frequency::kCount);

  PeriodicScheduler()
      : Task("Periodic_Scheduler", Priority::kLow), task_list_{ nullptr }
  {
    const uint32_t kPeriods[]  = { 1000, 100, 10, 1 };
    const char * timer_names[] = { "Timer_1Hz", "Timer_10Hz", "Timer_100Hz",
                                   "Timer_1000Hz" };
    for (uint8_t i = 0; i < kMaxTaskCount; i++)
    {
      timers_[i] =
          xTimerCreateStatic(timer_names[i],
                             kPeriods[i],          // timer period in ticks
                             pdTRUE,               // auto-reload
                             nullptr,              // pvTimerID
                             HandlePeriodicTimer,  // pxCallbackFunction
                             &(timer_buffers_[i]));
      SJ2_ASSERT_FATAL(timers_[i] != nullptr,
                       "Failed to create timer for: %s\n", timer_names[i]);
      SJ2_ASSERT_FATAL(xTimerStart(timers_[i], 0) == pdPASS,
                       "Failed to set timer into the active state for: %s\n",
                       timer_names[i]);
    }
  }
  bool Run() override
  {
    return true;
  }
  /// Adds a periodic task to the scheduler.
  ///
  /// @param task       Pointer to the task that is to be executed periodically.
  /// @param frequency  Desired frequency the task should be executed.
  void SetTask(PeriodicTaskInterface * task, Frequency frequency)
  {
    task_list_[Value(frequency)] = task;
    // Since HandlePeriodicTimer callback handles multiple timers, the pointer
    // of the task is stored as the timer's pvTimerID in order to be able to
    // identify which task should be handled in the HandlePeriodicTimer()
    // function.
    vTimerSetTimerID(timers_[Value(frequency)],
                     static_cast<void *>(task));
  }
  /// @return Returns the scheduled task for a specified frequency. If no task
  ///         has been scheduled then a nullptr is returned.
  PeriodicTaskInterface * GetTask(Frequency frequency) const
  {
    return task_list_[Value(frequency)];
  }

 protected:
  TimerHandle_t timers_[kMaxTaskCount];
  StaticTimer_t timer_buffers_[kMaxTaskCount];
  /// Array containing the 1Hz, 10Hz, 100Hz, and 1000Hz tasks.
  PeriodicTaskInterface * task_list_[kMaxTaskCount];

  static void HandlePeriodicTimer(TimerHandle_t timer)
  {
    PeriodicTaskInterface * task =
        static_cast<PeriodicTaskInterface *>(pvTimerGetTimerID(timer));
    if (task != nullptr)
    {
      xSemaphoreGive(task->GetPeriodicSemaphore());
    }
  }
};
}  // namespace rtos
}  // namespace sjsu

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
#include "utility/enum.hpp"
#include "utility/log.hpp"

namespace rtos
{
/// Pointer to the function containing the code to be executed periodically.
/// The functions should not containing any blocking code.
/// eg. vTaskDelay() and vTaskDelayUntil() should not be used within the
/// function.
///
/// @param count Number of times the task function has been run.
using PeriodicTaskFunction = void (*)(uint32_t count);

class PeriodicTaskInterface
{
 public:
  virtual PeriodicTaskFunction GetTaskFunction() const = 0;
  virtual SemaphoreHandle_t GetPeriodicSemaphore()     = 0;
  virtual uint32_t GetRunCount()                       = 0;
};

/// @tparam kTaskStackSize Task stack size in bytes.
template <size_t kTaskStackSize>
class PeriodicTask : public Task<kTaskStackSize>,
                     public virtual PeriodicTaskInterface
{
 public:
  PeriodicTask(const char * name, Priority priority,
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
  /// @return Returns the periodic semaphore that is used by the
  ///         PeriodicScheduler to manager the PeriodicTask
  SemaphoreHandle_t GetPeriodicSemaphore() override
  {
    return semaphore_;
  }
  /// @return Returns the number of times the task has been executed.
  uint32_t GetRunCount() override
  {
    return run_count_;
  }

 protected:
  PeriodicTaskFunction task_function_;
  /// Pointer references to statically allocated buffer for each
  /// semaphore in periodic_semaphores
  StaticSemaphore_t semaphore_buffer_;
  /// Periodic semaphore used by the PeriodicScheduler to ensure the task that
  /// doesnt overrun its allocated time.
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

  static constexpr size_t kMaxTaskCount = util::Value(Frequency::kCount);

  PeriodicScheduler()
      : Task("Periodic Scheduler", Priority::kHigh),
        task_list_{ nullptr },
        counters_{ 0 }
  {
    // The delay time should be 1 since the FreeRTOS tick rate is set to
    // 1 tick = 1000Hz and 1000Hz is fastest rate for the PeriodicScheduler
    SetDelayTime(1);
  }
  bool Run() override
  {
    // Since the FreeRTOS tick rate is set to 1000Hz, the 1000Hz task should be
    // executed every 1 tick and the 100Hz, 10Hz, and 1Hz tasks would execute at
    // 1/10th of the subsequent rates
    if (HandlePeriodicSemaphore(Frequency::k1000Hz, 1))
    {
      if (HandlePeriodicSemaphore(Frequency::k100Hz, 10))
      {
        if (HandlePeriodicSemaphore(Frequency::k10Hz, 10))
        {
          if (HandlePeriodicSemaphore(Frequency::k1Hz, 10))
          {
            // NOLINT
          }
        }
      }
    }
    return true;
  }
  /// Adds a periodic task to the scheduler.
  ///
  /// @param task       Pointer to the task that is to be executed periodically.
  /// @param frequency  Desired frequency the task should be executed.
  void SetTask(PeriodicTaskInterface * task, Frequency frequency)
  {
    task_list_[util::Value(frequency)] = task;
  }
  /// @return Returns the scheduled task for a specified frequency.
  PeriodicTaskInterface * GetTask(Frequency frequency) const
  {
    return task_list_[util::Value(frequency)];
  }

 protected:
  /// Array containing the 1Hz, 10Hz, 100Hz, and 1000Hz tasks.
  PeriodicTaskInterface * task_list_[kMaxTaskCount];
  /// Frequency counters used in HandlePeriodicSemaphore() to determine if a
  /// semphore should be given for a task to be executed.
  uint8_t counters_[kMaxTaskCount];
  /// Handles semaphore of the specified periodic task function to ensure the
  /// task does not overrun. Each time the function is invokes, the counter for
  /// the respective frequecy in counters_ is incremented.
  ///
  /// When the counter matches the period it is reset, at this time, if there is
  /// an existing scheduled task and the semaphore is taken before it can be
  /// given, then the task has exceeded its allocated run-time; otherwise, the
  /// semaphore is given to allow the task to be executed at the specified
  /// frequency.
  ///
  /// @param frequency  Frequency at which the task executes.
  /// @param period     Since the FreeRTOS frequency rate is set to 1000Hz,
  ///                   period = 1 would be 1 RTOS tick.
  /// @return           Returns true if the counters_[frequency] matches the
  ///                   specified period.
  bool HandlePeriodicSemaphore(Frequency frequency, uint8_t period)
  {
    bool semaphore_given = false;
    if (++counters_[util::Value(frequency)] == period)
    {
      counters_[util::Value(frequency)] = 0;
      PeriodicTaskInterface * task      = task_list_[util::Value(frequency)];
      // do nothing if no task is scheduled for the specified frequency
      if (task == nullptr)
      {
        return true;
      }
      SemaphoreHandle_t semaphore                = task->GetPeriodicSemaphore();
      [[maybe_unused]] const char * task_names[] = { "1Hz", "10Hz", "100Hz",
                                                     "1000Hz" };
      SJ2_ASSERT_FATAL(xSemaphoreTake(semaphore, 0) == false,
                       "Overrun occured for task with frequency: %s",
                       task_names[util::Value(frequency)]);
      semaphore_given = true;
      xSemaphoreGive(semaphore);
    }
    return semaphore_given;
  }
};
};  // namespace rtos

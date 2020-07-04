#pragma once

#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"

#include "L3_Application/task_scheduler.hpp"
#include "utility/enum.hpp"
#include "utility/log.hpp"

namespace sjsu
{
namespace rtos
{
/// An abstract interace for the PeriodicTask class.
class PeriodicTaskInterface
{
 public:
  /// Function containing the code to be executed periodically.
  ///
  /// @warning The function should not contain any blocking code. For example,
  ///          vTaskDelay() and vTaskDelayUntil() should not be used within the
  ///          function.
  ///
  /// @param count Number of times the task function has been run.
  using TaskFunction = std::function<void(uint32_t count)>;

  /// @return The task function that is executed periodically by the task.
  virtual TaskFunction * GetTaskFunction() const = 0;

  /// @return The semaphore used by the PeriodicScheduler to manage the
  ///         execution of the task function.
  virtual SemaphoreHandle_t GetPeriodicSemaphore() const = 0;

  /// @return The number of times the task has successfully executed.
  virtual uint32_t GetRunCount() const = 0;
};

/// An abstract layer for FreeRTOS tasks that are scheduled and executed at
/// periodic intervals by the PeriodicScheduler.
///
/// @attention This task inherits from the Task interface and must be persistent
///            or in global space.
///
/// @tparam kTaskStackSize Task stack size in bytes.
template <size_t kTaskStackSize>
class PeriodicTask final : public Task<kTaskStackSize>,
                           public virtual PeriodicTaskInterface
{
 public:
  /// @param name The name used to identify this task.
  /// @param priority The priority of this task.
  /// @param task_function The task function that is periodically executed.
  explicit PeriodicTask(const char * name,
                        Priority priority,
                        TaskFunction * task_function)
      : Task<kTaskStackSize>(name, priority), task_function_(task_function)
  {
    SJ2_ASSERT_FATAL(task_function_ != nullptr,
                     "Task function cannot be null for task: %s", name);
    semaphore_ = xSemaphoreCreateBinaryStatic(&semaphore_buffer_);
    SJ2_ASSERT_FATAL(semaphore_ != nullptr,
                     "Error creating periodic semaphore for task: %s", name);
    run_count_ = 0;
  }

  /// @returns Returns true each time the task_function_ is executed.
  bool Run() override
  {
    while (xSemaphoreTake(semaphore_, portMAX_DELAY))
    {
      (*task_function_)(++run_count_);
    }
    return true;
  }

  /// @return The task function that is executed periodically by the task.
  TaskFunction * GetTaskFunction() const override
  {
    return task_function_;
  }

  /// @return The semaphore used by the PeriodicScheduler to manage the
  ///         execution of the task function.
  SemaphoreHandle_t GetPeriodicSemaphore() const override
  {
    return semaphore_;
  }

  /// @return The number of times the task has successfully executed.
  uint32_t GetRunCount() const override
  {
    return run_count_;
  }

 protected:
  /// The function that is executed at a fixed frequency.
  TaskFunction * task_function_;
  /// The pre-allocated stack buffer of semaphore_.
  StaticSemaphore_t semaphore_buffer_;
  /// A semaphore managed by the PeriodicScheduler to control when the execution
  /// of the task_function_.
  SemaphoreHandle_t semaphore_;
  /// Number of times the task function has been executed.
  uint32_t run_count_;
};

/// A Task responsible for executing PeriodicTasks at fixed frequencies of 1Hz,
/// 10Hz, 100Hz, and 1000Hz.
///
/// @attention This task inherits from the Task interface and must be persistent
///            or in global space.
class PeriodicScheduler final : public Task<512>
{
 public:
  /// The available frequencies at which tasks can be scheduled.
  enum class Frequency : uint8_t
  {
    /// Frequency option to execute the periodic task at 1 Hz.
    kF1Hz = 0,
    /// Frequency option to execute the periodic task at 10 Hz.
    kF10Hz,
    /// Frequency option to execute the periodic task at 100 Hz.
    kF100Hz,
    /// Frequency option to execute the periodic task at 1000 Hz.
    kF1000Hz,
    /// The total number of frequency options.
    kCount,
  };

  /// The total number of PeriodicTasks that can be scheduled. One PeriodicTask
  /// can be scheduled for each frequency option.
  static constexpr size_t kMaxTaskCount = Value(Frequency::kCount);

  /// @param name The name used to identify this scheduler.
  explicit PeriodicScheduler(const char * name)
      : Task(name, Priority::kLow), task_list_{ nullptr }
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

  /// @returns Always returns true.
  bool Run() override
  {
    return true;
  }

  /// Adds a periodic task to the scheduler.
  ///
  /// @param task      Pointer to the task that is to be executed periodically.
  /// @param frequency Desired frequency the task should be executed.
  void SetTask(PeriodicTaskInterface * task, Frequency frequency)
  {
    SJ2_ASSERT_FATAL(task != nullptr, "The task must not be a nullptr.");
    task_list_[Value(frequency)] = task;
    // Since HandlePeriodicTimer callback handles multiple timers, the pointer
    // of the task is stored as the timer's pvTimerID in order to be able to
    // identify which task should be handled in the callback.
    vTimerSetTimerID(timers_[Value(frequency)], static_cast<void *>(task));
  }

  /// @return The scheduled task for a specified frequency. If no task has been
  ///         scheduled then a nullptr is returned.
  PeriodicTaskInterface * GetTask(Frequency frequency) const
  {
    return task_list_[Value(frequency)];
  }

 private:
  /// Callback handler that is invoked when a timer in the timers_'s array is
  /// timed out.
  ///
  /// @param timer_handle The handle of the timer that triggered this callback.
  static void HandlePeriodicTimer(TimerHandle_t timer_handle)
  {
    PeriodicTaskInterface * task =
        static_cast<PeriodicTaskInterface *>(pvTimerGetTimerID(timer_handle));
    if (task != nullptr)
    {
      xSemaphoreGive(task->GetPeriodicSemaphore());
    }
  }

  /// An array of four FreeRTOS timers used to manage task execution at 1Hz,
  /// 10Hz, 100Hz, and 1000Hz.
  TimerHandle_t timers_[kMaxTaskCount];
  /// Array of pre-allocated stack buffers for each timer in timer_.
  StaticTimer_t timer_buffers_[kMaxTaskCount];
  /// Array containing the 1Hz, 10Hz, 100Hz, and 1000Hz tasks.
  PeriodicTaskInterface * task_list_[kMaxTaskCount];
};
}  // namespace rtos
}  // namespace sjsu

#pragma once

#include "config.hpp"
#include "L0_Platform/ram.hpp"
#include "utility/log.hpp"
#include "utility/rtos.hpp"

namespace sjsu
{
namespace rtos
{
// Forward declaration of TaskScheduler for use in the TaskInterface.
class TaskScheduler;

// =============================================================================
// TaskInterface class
// =============================================================================

/// An abstract interface for the Task interface class.
///
/// @note The TaskInterface class is used to provide a generalization for the
///       Task class since the Task class takes in a template argument. The Task
///       interface class should be inherited instead of directly inheritting
///       TaskInterface when creating a FreeRTOS task class.
class TaskInterface
{
 protected:
  /// TaskScheduler responsible for scheduling this task.
  TaskScheduler * task_scheduler_;

 public:
  /// @param task_scheduler Reference to the TaskScheduler responsible for
  ///                       scheduling this task.
  void SetTaskScheduler(TaskScheduler * task_scheduler)
  {
    task_scheduler_ = task_scheduler;
  }

  /// @returns A reference to the TaskScheduler responsible for scheduling this
  ///          task.
  TaskScheduler * GetTaskScheduler() const
  {
    return task_scheduler_;
  }

  /// Setup is performed before the task begins to execute.
  /// The function should be overridden with any initialization code that the
  /// task requires.
  virtual bool Setup() = 0;

  /// Called once before Run() is invoked.
  virtual bool PreRun() = 0;

  /// Execute the task.
  virtual bool Run() = 0;

  /// Suspends the task until it is resumed.
  virtual void Suspend() const = 0;

  /// Resumes the task if it has been suspended.
  virtual void Resume() const = 0;

  /// Remove the task from the scheduler and delete the task.
  virtual void Delete() const = 0;

  /// @return The name of this task.
  virtual const char * GetName() const = 0;

  /// @return The priority of the task.
  virtual Priority GetPriority() const = 0;

  /// @return The pre-allocated stack size for the task in bytes.
  virtual size_t GetStackSize() const = 0;

  /// Set task handle for this task object.
  virtual void SetHandle(TaskHandle_t * handle) = 0;

  /// @return The task handle for this task object.
  virtual TaskHandle_t * GetHandle() = 0;

  /// @return A pointer to the task's statically allocated buffer.
  virtual StaticTask_t * GetTaskBuffer() = 0;

  /// @return A pointer reference of the task's statically allocated stack.
  virtual StackType_t * GetStack() = 0;

  /// Sets the delay time to ensure Run() is called at a desired frequency for
  /// periodic tasks.
  ///
  /// For examples, if time = 1000 and the sjsu::rtos tick period = 1ms, then
  /// Run() will be called every 1 second.
  ///
  /// @param time Desired delay time in sjsu::rtos ticks.
  virtual void SetDelayTime(uint32_t time) = 0;

  /// @return Returns the delay time in sjsu::rtos ticks.
  virtual uint32_t GetDelayTime() const = 0;
};

// =============================================================================
// TaskScheduler class
// =============================================================================

/// A FreeRTOS task scheduler responsible for scheduling tasks that inherit the
/// Task interface.Tasks inheriting Task interface are automatically added to
/// the scheduler when constructed.
class TaskScheduler final
{
 public:
  TaskScheduler()
      : task_list_{ nullptr },
        task_count_(0),
        pre_run_event_group_handle_(NULL),
        pre_run_sync_bits_(0x0)
  {
  }

  /// @return The PreRun event group handler used to notify that PreRun of all
  ///         tasks have completed successfully.
  EventGroupHandle_t GetPreRunEventGroupHandle() const
  {
    return pre_run_event_group_handle_;
  }

  /// @return The sync bits of the PreRun event group.
  EventBits_t GetPreRunSyncBits() const
  {
    return pre_run_sync_bits_;
  }

  /// @return The current number of scheduled tasks.
  uint8_t GetTaskCount() const
  {
    return task_count_;
  }

  /// Add a task to the task scheduler. If the scheduler is full, the task will
  /// not be added and a fatal error will be asserted.
  ///
  /// @note When a task inheriting the TaskInterface is constructed, it will
  ///       automatically call this function to add itself to the scheduler.
  ///
  /// @param task Task to add.
  void AddTask(TaskInterface * task)
  {
    SJ2_ASSERT_FATAL(
        task_count_ + 1 < config::kTaskSchedulerSize,
        "The scheduler is currently full, the task will not be "
        "added. Consider increasing the scheduler size configuration.");
    for (uint8_t i = 0; i < config::kTaskSchedulerSize; i++)
    {
      if (task_list_[i] == nullptr)
      {
        task_list_[i] = task;
        task_count_++;
        task->SetTaskScheduler(this);
        return;
      }
    }
  }

  /// Removes a specified task by its name and updates the task_list_ and
  /// task_count_.
  ///
  /// @param task_name Name of the task to remove.
  void RemoveTask(const char * task_name)
  {
    const uint8_t kTaskIndex = GetTaskIndex(task_name);
    if (kTaskIndex > config::kTaskSchedulerSize)
    {
      return;
    }
    TaskHandle_t handle = task_list_[kTaskIndex]->GetHandle();
    if (handle != nullptr)
    {
      vTaskDelete(handle);
    }
    task_list_[kTaskIndex] = nullptr;
    task_count_--;
  }

  /// Retreive a task by its task name.
  ///
  /// @param task_name Name of the task.
  /// @return A nullptr if the task does not exist. Otherwise, returns a pointer
  ///         reference to the retrieved task with the matching name.
  TaskInterface * GetTask(const char * task_name) const
  {
    const uint32_t kTaskIndex = GetTaskIndex(task_name);
    if (kTaskIndex > task_count_)
    {
      return nullptr;
    }
    return task_list_[kTaskIndex];
  }

  /// Used to get a task's index to determine the sync bit for the PreRun event
  /// group.
  ///
  /// @param task_name Name of the task.
  /// @return The index of the specified task. If the task is not scheduled,
  ///         kTaskSchedulerSize + 1 will be returned.
  uint8_t GetTaskIndex(const char * task_name) const
  {
    for (uint8_t i = 0; i < config::kTaskSchedulerSize; i++)
    {
      if (task_list_[i] != nullptr)
      {
        if (!strcmp(task_list_[i]->GetName(), task_name))
        {
          return i;
        }
      }
    }
    return static_cast<uint8_t>(config::kTaskSchedulerSize + 1);
  }

  /// @return A pointer reference to an immutable array of all currently
  ///         scheduled tasks.
  TaskInterface * const * GetAllTasks() const
  {
    return task_list_;
  }

  /// Starts the scheduler and attempts to initialize all tasks.
  /// If there are currently no tasks scheduled, a fatal error will be
  /// asserted.
  void Start()
  {
    SJ2_ASSERT_FATAL(
        task_count_ != 0,
        "Attempting to start TaskScheduler but there are no tasks that "
        "are currently scheduled.");
    InitializeAllTasks();
    vTaskStartScheduler();
    // does not reach this point
  }

 private:
  /// Function used during InitializeAllTasks() for xTaskCreate() for running
  /// scheduled tasks.
  ///
  /// @param task_pointer Pointer reference of the task to run.
  static void RunTask(void * task_pointer)
  {
    TaskInterface & task = *(reinterpret_cast<TaskInterface *>(task_pointer));
    TaskScheduler & task_scheduler = *(task.GetTaskScheduler());

    const uint8_t kTaskCount = task_scheduler.GetTaskCount();
    const uint8_t kTaskIndex = task_scheduler.GetTaskIndex(task.GetName());
    SJ2_ASSERT_FATAL(kTaskIndex < kTaskCount,
                     "The task index should not exceed the task count.");
    // Perform PreRun for the task and then set the event group sync bit to
    // broadcast this task's PreRun has completed
    EventGroupHandle_t pre_run_event_group_handle =
        task_scheduler.GetPreRunEventGroupHandle();
    const EventBits_t kPreRunSyncBits = task_scheduler.GetPreRunSyncBits();
    const uint32_t kSyncBit           = (1 << kTaskIndex);
    SJ2_ASSERT_FATAL(task.PreRun(),
                     "PreRun() failed for task: %s, terminating scheduler!",
                     task.GetName());
    // wait for all other PreRun() of other tasks to finish
    xEventGroupSync(pre_run_event_group_handle, kSyncBit, kPreRunSyncBits,
                    portMAX_DELAY);
    // All PreRun() complete, each Task's Run() can now start executing...
    TickType_t last_wake_time = xTaskGetTickCount();

    while (true)
    {
      if (!task.Run())
      {
        LogWarning(
            "An error occurred, the following task will be suspended: %s",
            task.GetName());
        vTaskSuspend(NULL);
      }
      // delay task if the task's delay time has been set...
      uint32_t delay_time = task.GetDelayTime();
      if (delay_time)
      {
        vTaskDelayUntil(&last_wake_time, delay_time);
      }
    }
  }

  /// Attempt to initialize each scheduled task with xTaskCreate() and execute
  /// each task's Setup(). A fatal error is asserted if either
  /// xTaskCreateStatic() or Setup() fails.
  void InitializeAllTasks()
  {
    for (uint32_t i = 0; i < task_count_; i++)
    {
      TaskInterface * task = task_list_[i];
      *(task->GetHandle()) = xTaskCreateStatic(
          RunTask,  // function to execute the task
          task->GetName(),
          static_cast<uint16_t>(StackSize(task->GetStackSize())),
          PassParameter(task),  // pointer of task to run
          task->GetPriority(),
          task->GetStack(),        // the task's statically allocated memory
          task->GetTaskBuffer());  // task TCB
      SJ2_ASSERT_FATAL(task->GetHandle() != nullptr,
                       "Unable to create task: %s", task->GetName());
      SJ2_ASSERT_FATAL(task->Setup(), "Failed to complete Setup() for task: %s",
                       task->GetName());
      pre_run_sync_bits_ |= (1 << i);
    }
    pre_run_event_group_handle_ =
        xEventGroupCreateStatic(&pre_run_event_group_buffer_);
    SJ2_ASSERT_FATAL(pre_run_event_group_handle_ != nullptr,
                     "Failed to create PreRun Event Group!");
  }

  /// Array containing all scheduled tasks.
  TaskInterface * task_list_[config::kTaskSchedulerSize];
  /// Current number of scheduled tasks in task_list_.
  uint8_t task_count_;
  /// Buffer to hold the static allocation of the PreRun Event Group.
  StaticEventGroup_t pre_run_event_group_buffer_;
  /// Event Group handle to be used by tasks running their PreRun callback to
  /// indicate that all of them have completed it.
  EventGroupHandle_t pre_run_event_group_handle_;
  /// All PreRun sync bits for PreRun Event Group.
  EventBits_t pre_run_sync_bits_;
};

// =============================================================================
// Task interface class
// =============================================================================

/// An abstraction layer for FreeRTOS tasks. All tasks must inherit this
/// interface and override the Run() function.
///
/// @attention All tasks must be persistent or in global space.
///
/// @tparam kStackSize The pre-allocated stack size of this task in bytes.
template <size_t kStackSize>
class Task : public TaskInterface
{
 public:
  /// Setup is performed before the task begins to execute.
  /// The function should be overridden with any initialization code that the
  /// task requires.
  bool Setup() override
  {
    return true;
  }

  /// Called once before Run() is invoked.
  bool PreRun() override
  {
    return true;
  }

  /// Suspends the task until it is resumed.
  void Suspend() const override
  {
    vTaskSuspend(handle_);
  }

  /// Resumes the task if it has been suspended.
  void Resume() const override
  {
    vTaskResume(handle_);
  }

  /// Remove the task from the scheduler and delete the task.
  void Delete() const override
  {
    vTaskSuspend(handle_);
    task_scheduler_->RemoveTask(kName);
  }

  /// @return The name of this task.
  const char * GetName() const override
  {
    return kName;
  }

  /// @return The priority of the task.
  Priority GetPriority() const override
  {
    return kPriority;
  }

  /// @return The pre-allocated stack size for the task in bytes.
  size_t GetStackSize() const override
  {
    return kStackSize;
  }

  /// Set task handle for this task object.
  void SetHandle(TaskHandle_t * handle) override
  {
    handle_ = handle;
  }

  /// @return The task handle for this task object.
  TaskHandle_t * GetHandle() override
  {
    return &handle_;
  }

  /// @return A pointer to the task's statically allocated buffer.
  StaticTask_t * GetTaskBuffer() override
  {
    return &task_buffer_;
  }

  /// @return A pointer reference of the task's statically allocated stack.
  StackType_t * GetStack() override
  {
    return stack_;
  }

  /// Sets the delay time to ensure Run() is called at a desired frequency for
  /// periodic tasks.
  ///
  /// For examples, if time = 1000 and the sjsu::rtos tick period = 1ms, then
  /// Run() will be called every 1 second.
  ///
  /// @param time Desired delay time in sjsu::rtos ticks.
  void SetDelayTime(uint32_t time) override
  {
    delay_time_ = time;
  }

  /// @return The delay time in sjsu::rtos ticks.
  uint32_t GetDelayTime() const override
  {
    return delay_time_;
  }

  /// Default destructor.
  virtual ~Task() {}

 protected:
  /// Default constructor. When a Task is constructed, it is automatically
  /// added to the specified TaskScheduler.
  ///
  /// @param name Name used to easily identify the task.
  /// @param priority Priority of the task.
  explicit constexpr Task(const char * name, Priority priority)
      : kName(name), kPriority(priority), handle_(NULL), delay_time_(0)
  {
    DeclaredOnStackCheck();
  }

  /// Checks if the object was statically allocated either in the .data, .bss,
  /// or on the heap. Returns false if the position of this object is not within
  /// the bounds of those sections meaning it must be on the heap, which means
  /// that the object may not live for the total lifetime of program. This
  /// usually results in a crash at some point in the code.
  bool DeclaredOnStackCheck()
  {
    if constexpr (build::kPlatform != build::Platform::linux &&
                  build::kPlatform != build::Platform::host)
    {
      // This task's position in memory
      intptr_t address = reinterpret_cast<intptr_t>(this);
      // The .data section starts at RAM address 0. The .bss section follows
      // after the .data section. The last variable in the .bss is the also the
      // end of the data section.
      intptr_t end_of_data_and_bss =
          reinterpret_cast<intptr_t>(bss_section_table[0].ram_location) +
          static_cast<intptr_t>(bss_section_table[0].length);

      intptr_t start_of_heap = reinterpret_cast<intptr_t>(&heap);
      intptr_t end_of_heap   = reinterpret_cast<intptr_t>(&heap_end);

      sjsu::LogDebug("This Task's Address: 0x%08X", address);
      sjsu::LogDebug("End of .data & .bss: 0x%08X", end_of_data_and_bss);
      sjsu::LogDebug("Start of Heap      : 0x%08X", start_of_heap);
      sjsu::LogDebug("End of Heap        : 0x%08X", end_of_heap);

      SJ2_ASSERT_FATAL(
          address < end_of_data_and_bss ||
              (start_of_heap <= address && address <= end_of_heap),
          "Must define tasks globally or within heap using new or malloc. "
          "Cannot exist on the stack.\n");
    }
    return true;
  }

  /// Holds a pointer to the name of the task.
  const char * const kName;
  /// Holds the task's priority. This is constant so it will not reflect if this
  /// task has a priority that has been elevated due to priority inheritance.
  const Priority kPriority;
  /// Used to identify the task
  TaskHandle_t handle_;
  /// Task delay time in sjsu::rtos ticks.
  uint32_t delay_time_;
  /// Pointer reference to the statically allocated buffer that will hold the
  /// task TCB.
  StaticTask_t task_buffer_;
  /// Pointer reference to the pre-allocated stack.
  StackType_t stack_[kStackSize];
};
}  // namespace rtos
}  // namespace sjsu

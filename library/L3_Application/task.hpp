#pragma once

#include "L0_Platform/ram.hpp"
#include "L3_Application/task_scheduler.hpp"

namespace sjsu
{
namespace rtos
{
/// An abstract interface for the Task interface class.
class TaskInterface
{
 public:
  /// @return The TaskScheduler responsible for scheduling this task.
  virtual TaskSchedulerInterface & GetTaskScheduler() const = 0;
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
  /// @return The TaskScheduler responsible for scheduling this task.
  TaskSchedulerInterface & GetTaskScheduler() const override
  {
    return task_scheduler_;
  }
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
    task_scheduler_.RemoveTask(kName);
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
  /// @return Returns the delay time in sjsu::rtos ticks.
  uint32_t GetDelayTime() const override
  {
    return delay_time_;
  }
  virtual ~Task() {}

 protected:
  /// Default constructor. When a Task is constructed, it is automatically
  /// added to the specified TaskScheduler.
  ///
  /// @param name           Name used to easily identify the task.
  /// @param priority       Priority of the task.
  /// @param task_scheduler TaskScheduler responsible for scheduling this task.
  explicit constexpr Task(const char * name,
                          Priority priority,
                          TaskSchedulerInterface & task_scheduler)
      : task_scheduler_(task_scheduler),
        kName(name),
        kPriority(priority),
        handle_(NULL),
        delay_time_(0)
  {
    DeclaredOnStackCheck();
    task_scheduler_.AddTask(this);
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

      LOG_DEBUG("This Task's Address: 0x%08X", address);
      LOG_DEBUG("End of .data & .bss: 0x%08X", end_of_data_and_bss);
      LOG_DEBUG("Start of Heap      : 0x%08X", start_of_heap);
      LOG_DEBUG("End of Heap        : 0x%08X", end_of_heap);

      SJ2_ASSERT_FATAL(
          address < end_of_data_and_bss ||
              (start_of_heap <= address && address <= end_of_heap),
          "Must define tasks globally or within heap using new or malloc. "
          "Cannot exist on the stack.\n");
    }
    return true;
  }

  /// TaskScheduler responsible for scheduling this task.
  TaskSchedulerInterface & task_scheduler_;
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

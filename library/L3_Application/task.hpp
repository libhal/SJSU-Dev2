// This file contains the TaskInterface class.
// All tasks must inherit TaskInterface and override the Run() function.
//
// NOTE: All tasks must be persistent or in global space.
//
// Usage:
//      class PrinterTask : public sjsu::rtos::Task<1024>;
//      PrinterTask printer_one("Printer A", "I am a printer, I am faster");
//      printer_one.SetDelayTime(500);
//      sjsu::rtos::TaskScheduler::Instance().Start();
#pragma once

#include <cstdint>

#include "L0_Platform/ram.hpp"
#include "L3_Application/task_scheduler.hpp"
#include "utility/macros.hpp"

namespace sjsu
{
namespace rtos
{

class TaskInterface
{
 public:
  virtual bool Setup()                          = 0;
  virtual bool PreRun()                         = 0;
  virtual bool Run()                            = 0;
  virtual void Suspend()                        = 0;
  virtual void Resume()                         = 0;
  virtual void Delete()                         = 0;
  virtual const char * GetName()                = 0;
  virtual Priority GetPriority()                = 0;
  virtual size_t GetStackSize()                 = 0;
  virtual void SetHandle(TaskHandle_t * handle) = 0;
  virtual TaskHandle_t * GetHandle()            = 0;
  virtual StaticTask_t * GetTaskBuffer()        = 0;
  virtual StackType_t * GetStack()              = 0;
  virtual void SetDelayTime(uint32_t time)      = 0;
  virtual uint32_t GetDelayTime()               = 0;
};

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
  void Suspend() override
  {
    vTaskSuspend(handle_);
  }
  /// Resumes the task if it has been suspened.
  void Resume() override
  {
    vTaskResume(handle_);
  }
  /// Remove the task from the scheduler and delete the task.
  void Delete() override
  {
    vTaskSuspend(handle_);
    sjsu::rtos::TaskScheduler::Instance().RemoveTask(kName);
  }
  /// Return name of this task.
  const char * GetName() override
  {
    return kName;
  }
  /// Returns the priority of the task.
  Priority GetPriority() override
  {
    return kPriority;
  }
  /// @return Returns the allocated stack size for the task in kilobytes.
  size_t GetStackSize() override
  {
    return kStackSize;
  }
  /// Set task handler for this task object.
  void SetHandle(TaskHandle_t * handle) override
  {
    handle_ = handle;
  }
  /// Retrieve task handler from this task object.
  TaskHandle_t * GetHandle() override
  {
    return &handle_;
  }
  /// Returns a pointer to the task's statically allocated buffer.
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
  uint32_t GetDelayTime() override
  {
    return delay_time_;
  }
  virtual ~Task() {}

 protected:
  /// Default constructor. When a Task is constructed, it is automatically
  /// added to the TaskScheduler singleton.
  ///
  /// @param name       Name used to easily identify the task.
  /// @param priority   Priority of the task.
  constexpr Task(const char * name, Priority priority)
      : kName(name), kPriority(priority), handle_(NULL), delay_time_(0)
  {
    DeclaredOnStackCheck();
    sjsu::rtos::TaskScheduler::Instance().AddTask(this);
  }
  /// Checks if the object was statically allocated either in the .data, .bss,
  /// or on the heap. Returns false if the position of this object is not within
  /// the bounds of those sections meaning it must be on the heap, which means
  /// that the object may not live for the total lifetime of program. This
  /// usually results in a crash at some point in the code.
  bool DeclaredOnStackCheck()
  {
#if !defined(HOST_TEST) && !defined(__unix__)
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
#endif
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
  // Pointer reference to the statically allocated buffer that will hold the
  // task TCB.
  StaticTask_t task_buffer_;
  // Pointer reference to the allocated stack with the size specified by
  // stack_size_.
  StackType_t stack_[kStackSize];
};

}  // namespace rtos
}  // namespace sjsu

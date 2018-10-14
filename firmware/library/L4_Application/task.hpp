// This file contains the TaskInterface class.
// All tasks must inherit TaskInterface and override the Run() function.
// Usage:
//      class PrinterTask : public rtos::Task<1024>;
//      PrinterTask * printer_one =
//          new PrinterTask("Printer A", "I am a printer, I am faster");
//      printer_one->SetDelayTime(500);
//      rtos::TaskScheduler::Instance().Start();
#pragma once

#include <cstdint>

#include "L2_Utilities/macros.hpp"
#include "L4_Application/task_scheduler.hpp"

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
  virtual ~Task(){};
  // Setup is performed before the task begins to execute.
  // The function should be overridden with any initialization code that the
  // task requires.
  bool Setup() override
  {
    return true;
  };
  // Called once before Run() is invoked.
  bool PreRun() override
  {
    return true;
  };
  // Suspends the task until it is resumed.
  inline void Suspend() override
  {
    vTaskSuspend(handle_);
  };
  // Resumes the task if it has been suspened.
  inline void Resume() override
  {
    vTaskResume(handle_);
  };
  // Remove the task from the scheduler and delete the task.
  inline void Delete() override
  {
    vTaskSuspend(handle_);
    rtos::TaskScheduler::Instance().RemoveTask(kName);
  };
  inline const char * GetName() override
  {
    return kName;
  };
  inline Priority GetPriority() override
  {
    return kPriority;
  };
  // @return Returns the allocated stack size for the task in kilobytes.
  inline size_t GetStackSize() override
  {
    return kStackSize;
  };
  inline void SetHandle(TaskHandle_t * handle) override
  {
    handle_ = handle;
  };
  inline TaskHandle_t * GetHandle() override
  {
    return &(handle_);
  };
  inline StaticTask_t * GetTaskBuffer() override
  {
    return &(task_buffer_);
  };
  // @return A pointer reference of the task's statically allocated stack.
  inline StackType_t * GetStack() override
  {
    return stack_;
  };
  // Sets the delay time to ensure Run() is called at a desired frequency for
  // periodic tasks.
  //
  // For examples, if time = 1000 and the rtos tick period = 1ms, then Run()
  // will be called every 1 second.
  //
  // @param time Desired delay time in rtos ticks.
  inline void SetDelayTime(uint32_t time) override
  {
    delay_time_ = time;
  };
  // @return Returns the delay time in rtos ticks.
  inline uint32_t GetDelayTime() override
  {
    return delay_time_;
  };

 protected:
  const char * const kName;
  const Priority kPriority;
  // Used to identify the task
  TaskHandle_t handle_;
  // Task delay time in rtos ticks.
  uint32_t delay_time_;
  // Pointer reference to the statically allocated buffer that will hold the
  // task TCB.
  StaticTask_t task_buffer_;
  // Pointer reference to the allocated stack with the size specified by
  // stack_size_.
  StackType_t stack_[kStackSize];
  // Defualt constructor. When a Task is constructed, it is automatically
  // added to the TaskScheduler singleton.
  //
  // @param name       Name used to easily identify the task.
  // @param priority   Priority of the task.
  constexpr Task(const char * name, Priority priority)
      : kName(name), kPriority(priority), handle_(NULL), delay_time_(0)
  {
    rtos::TaskScheduler::Instance().AddTask(this);
  };
};
};  // namespace rtos

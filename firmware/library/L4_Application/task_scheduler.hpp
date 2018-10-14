// This file contains the TaskScheduler class used to scheduler task classes
// that inherit TaskInterface.
// Tasks inheriting TaskInterface are automatically added to the scheduler when
// constructed. Simply start the scheduler after constructing and configuring
// desired tasks.
// Usage:
//      rtos::TaskScheduler::Instance().Start();
#pragma once

#include <cstddef>
#include <cstdint>

#include "config.hpp"
#include "L2_Utilities/macros.hpp"
#include "L2_Utilities/rtos.hpp"
// Must include FreeRTOS before event_groups.h
#include "event_groups.h"

namespace rtos
{
class TaskInterface;

class TaskSchedulerInterface
{
 public:
  virtual EventGroupHandle_t GetPreRunEventGroupHandle()  = 0;
  virtual EventBits_t GetPreRunSyncBits()                 = 0;
  virtual uint8_t GetTaskCount()                          = 0;
  virtual void AddTask(TaskInterface * task)              = 0;
  virtual void RemoveTask(const char * task_name)         = 0;
  virtual TaskInterface * GetTask(const char * task_name) = 0;
  virtual uint8_t GetTaskIndex(const char * task_name)    = 0;
  virtual void Start()                                    = 0;
  virtual void InitializeAllTasks()                       = 0;
};

class TaskScheduler final : public TaskSchedulerInterface
{
 public:
  static TaskScheduler & Instance()
  {
    static TaskScheduler instance;
    return instance;
  };
  inline EventGroupHandle_t GetPreRunEventGroupHandle() override
  {
    return pre_run_event_group_handle_;
  };
  inline EventBits_t GetPreRunSyncBits() override
  {
    return pre_run_sync_bits_;
  };
  // @return Returns the current number of scheduled tasks.
  inline uint8_t GetTaskCount() override
  {
    return task_count_;
  };
  // Add a task to the task scheduler. If the scheduler is full, the task will
  // not be added and a fatal error will be asserted.
  //
  // @note When a task inheriting the TaskInterface is constructed, it will
  // automatically call this function to add itself to the scheduler.
  //
  // @param task  Task to add.
  void AddTask(TaskInterface * task) override
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
        return;
      }
    }
  };
  // Removes a specified task by its name and updates the task_list_ and
  // task_count_.
  // @param task_name  Name of the task to remove.
  void RemoveTask(const char * task_name) override;
  // Retreive a task by its task name.
  //
  // @param   task_name Name of the task.
  // @return  Returns nullptr if the task does not exist. Otherwise, returns a
  //          pointer reference to the retreived task with the matching name.
  TaskInterface * GetTask(const char * task_name) override
  {
    const uint32_t kTaskIndex = GetTaskIndex(task_name);
    if (kTaskIndex > task_count_)
    {
      return nullptr;
    }
    return task_list_[kTaskIndex];
  };
  // Used to get a task's index to determine the sync bit for the PreRun event
  // group.
  //
  // @param   task_name Name of the task.
  // @return  Returns the index of the specified task. If the task is not
  //          scheduled, kTaskSchedulerSize + 1 will be returned.
  uint8_t GetTaskIndex(const char * task_name) override;
  // @return Returns a pointer reference to all currently scheduled tasks.
  inline TaskInterface ** GetAllTasks()
  {
    return task_list_;
  };
  // Starts the scheduler and attempts to initialize all tasks.
  // If there are currently no tasks scheduled, a fatal error will be
  // asserted.
  void Start() override
  {
    SJ2_ASSERT_FATAL(
        task_count_ != 0,
        "Attempting to start TaskScheduler but there are no tasks that "
        "are currently scheduled.");
    InitializeAllTasks();
    vTaskStartScheduler();
    // does not reach this point
  };

 private:
  // Array containing all scheduled tasks.
  TaskInterface * task_list_[config::kTaskSchedulerSize];
  // Current number of scheduled tasks in task_list_.
  uint8_t task_count_;
  // Buffer to hold the static allocation of the PreRun Event Group.
  StaticEventGroup_t pre_run_event_group_buffer_;
  // Event Group handle of for notfiying all PreRun are complete.
  EventGroupHandle_t pre_run_event_group_handle_;
  // All PreRun sync bits for PreRun Event Group.
  EventBits_t pre_run_sync_bits_;
  // Function used during InitalizeAllTasks() for xTaskCreate() for running
  // scheduled tasks.
  //
  // @param task_ptr Pointer reference of the task to run.
  static void RunTask(void * task_ptr);

  TaskScheduler()
      : task_count_(0),
        pre_run_event_group_handle_(NULL),
        pre_run_sync_bits_(0x0){};
  // Attempt to initialize each scheduled task with xTaskCreate() and execute
  // each task's Setup().
  // A fatal error is asserted if either xTaskCreate() or Setup() fails.
  void InitializeAllTasks() override;
};
};  // namespace rtos

#include "L4_Application/task_scheduler.hpp"

#include <cstring>

#include "L4_Application/task.hpp"

void rtos::TaskScheduler::RunTask(void * task_ptr)
{
  TaskInterface & task = *(reinterpret_cast<TaskInterface *>(task_ptr));

  const uint8_t kTaskCount = TaskScheduler::Instance().GetTaskCount();
  const uint8_t kTaskIndex =
      TaskScheduler::Instance().GetTaskIndex(task.GetName());
  SJ2_ASSERT_FATAL(kTaskIndex < kTaskCount,
                   "The task index should not exceed the task count.");
  // Perform PreRun for the task and then set the event group sync bit to
  // broadcast this task's PreRun has completed
  EventGroupHandle_t pre_run_event_group_handle =
      TaskScheduler::Instance().GetPreRunEventGroupHandle();
  const EventBits_t kPreRunSyncBits =
      TaskScheduler::Instance().GetPreRunSyncBits();
  const uint32_t kSyncBit = (1 << kTaskIndex);
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
      SJ2_ASSERT_WARNING(
          false, "An error occured, the following task will be suspended: %s",
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
};

void rtos::TaskScheduler::InitializeAllTasks()
{
  for (uint32_t i = 0; i < task_count_; i++)
  {
    TaskInterface * task = task_list_[i];
    *(task->GetHandle()) = xTaskCreateStatic(
        RunTask,  // function to execute the task
        task->GetName(), static_cast<uint16_t>(StackSize(task->GetStackSize())),
        PassParameter(task),  // pointer of task to run
        task->GetPriority(),
        task->GetStack(),        // the task's statically allocated memory
        task->GetTaskBuffer());  // task TCB
    SJ2_ASSERT_FATAL(task->GetHandle() != nullptr, "Unable to create task: %s",
                     task->GetName());
    SJ2_ASSERT_FATAL(task->Setup(), "Failed to complete Setup() for task: %s",
                     task->GetName());
    pre_run_sync_bits_ |= (1 << i);
  }
  pre_run_event_group_handle_ =
      xEventGroupCreateStatic(&pre_run_event_group_buffer_);
  SJ2_ASSERT_FATAL(pre_run_event_group_handle_ != nullptr,
                   "Failed to create PreRun Event Group!");
};

void rtos::TaskScheduler::RemoveTask(const char * task_name)
{
  const uint8_t kTaskIndex = GetTaskIndex(task_name);
  if (kTaskIndex > config::kTaskSchedulerSize)
  {
    return;
  }
  vTaskDelete(task_list_[kTaskIndex]->GetHandle());
  task_list_[kTaskIndex] = nullptr;
  task_count_--;
};

uint8_t rtos::TaskScheduler::GetTaskIndex(const char * task_name)
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
};

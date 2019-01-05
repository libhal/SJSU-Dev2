// Tests for the TaskScheduler singleton class.
#include "L3_Application/task.hpp"

#include <iterator>

#include "L4_Testing/testing_frameworks.hpp"

EventGroupHandle_t test_event_group_handle;
EventGroupHandle_t xEventGroupCreateStatic_custom_fake(  // NOLINT
    StaticEventGroup_t *)
{
  return test_event_group_handle;
}

TEST_CASE("Testing TaskScheduler", "[task_scheduler]")
{
  using namespace rtos;  // NOLINT

  const char * task_names[] = {
    "Task 1",  "Task 2",  "Task 3",  "Task 4",  "Task 5",  "Task 6",
    "Task 7",  "Task 8",  "Task 9",  "Task 10", "Task 11", "Task 12",
    "Task 13", "Task 14", "Task 15", "Task 16", "Task 17",
  };
  constexpr uint8_t kTotalTestTaskCount          = std::size(task_names);
  TaskHandle_t task_handles[kTotalTestTaskCount] = { NULL };
  Mock<TaskInterface> mock_tasks[kTotalTestTaskCount];

  // stub each mock task
  for (uint8_t i = 0; i < kTotalTestTaskCount; i++)
  {
    When(Method(mock_tasks[i], GetName)).AlwaysReturn(task_names[i]);
    When(Method(mock_tasks[i], Setup)).AlwaysReturn(true);
    When(Method(mock_tasks[i], Run)).AlwaysReturn(true);
    When(Method(mock_tasks[i], GetHandle)).AlwaysReturn(&task_handles[i]);
    When(Method(mock_tasks[i], GetStackSize)).AlwaysReturn(0);
    When(Method(mock_tasks[i], GetPriority)).AlwaysReturn(Priority::kLow);
    When(Method(mock_tasks[i], GetStack)).AlwaysReturn(0);
    When(Method(mock_tasks[i], GetTaskBuffer)).AlwaysReturn(0);
  }

  SECTION("AddTask")
  {
    TaskScheduler scheduler         = TaskScheduler::Instance();
    TaskInterface ** task_list      = scheduler.GetAllTasks();
    constexpr uint8_t kMaxTaskCount = config::kTaskSchedulerSize;
    uint8_t task_count              = 0;
    // scheduler should be initially empty
    CHECK(scheduler.GetTaskCount() == 0);
    for (uint8_t i = 0; i < kMaxTaskCount; i++)
    {
      TaskInterface & task = mock_tasks[i].get();
      scheduler.AddTask(&task);
      task_count++;
      CHECK(scheduler.GetTaskCount() == task_count);
      CHECK(!strcmp(task_list[i]->GetName(), task.GetName()));
    }
    // scheduler should now be full and new tasks should not be added
    TaskInterface & task = mock_tasks[kMaxTaskCount].get();
    scheduler.AddTask(&task);
    CHECK(scheduler.GetTaskCount() == task_count);
    CHECK(strcmp(task_list[kMaxTaskCount - 1]->GetName(), task.GetName()));
  }
  SECTION("GetTask")
  {
    TaskScheduler scheduler         = TaskScheduler::Instance();
    constexpr uint8_t kMaxTaskCount = config::kTaskSchedulerSize;
    uint8_t task_count              = 0;
    CHECK(scheduler.GetTaskCount() == 0);
    for (uint8_t i = 0; i < kMaxTaskCount; i++)
    {
      TaskInterface & task = mock_tasks[i].get();
      scheduler.AddTask(&task);
      task_count++;
      CHECK(scheduler.GetTaskCount() == task_count);
      TaskInterface * retreived_task = scheduler.GetTask(task_names[i]);
      CHECK(retreived_task != nullptr);
      CHECK(!strcmp(retreived_task->GetName(), task.GetName()));
    }
    TaskInterface * non_existent_task =
        scheduler.GetTask(task_names[kMaxTaskCount]);
    CHECK(non_existent_task == nullptr);
  }
  SECTION("GetTaskIndex")
  {
    TaskScheduler scheduler         = TaskScheduler::Instance();
    constexpr uint8_t kMaxTaskCount = config::kTaskSchedulerSize;
    uint8_t task_count              = 0;
    for (uint8_t i = 0; i < kMaxTaskCount; i++)
    {
      TaskInterface & task = mock_tasks[i].get();
      scheduler.AddTask(&task);
      CHECK(scheduler.GetTaskIndex(task.GetName()) == task_count);
      task_count++;
      CHECK(scheduler.GetTaskCount() == task_count);
    }
    // If retreiving the index of a task that is not scheduled, GetTaskIndex
    // should return kTaskSchedulerSize + 1
    CHECK(scheduler.GetTaskIndex("Does not exist") == (kMaxTaskCount + 1));
  }
  SECTION("RemoveTask when scheduler is empty")
  {
    TaskScheduler scheduler    = TaskScheduler::Instance();
    // should do nothing since this task is not scheduled
    scheduler.RemoveTask("Task A");
    CHECK(vTaskDelete_fake.call_count == 0);
  }
  SECTION("RemoveTask when scheduler is not empty")
  {
    TaskScheduler scheduler    = TaskScheduler::Instance();
    TaskInterface ** task_list = scheduler.GetAllTasks();
    scheduler.AddTask(&(mock_tasks[0].get()));
    scheduler.AddTask(&(mock_tasks[1].get()));
    scheduler.AddTask(&(mock_tasks[2].get()));
    scheduler.AddTask(&(mock_tasks[3].get()));
    CHECK(scheduler.GetTaskCount() == 4);
    scheduler.RemoveTask(mock_tasks[2].get().GetName());
    CHECK(scheduler.GetTaskCount() == 3);
    CHECK(task_list[2] == nullptr);
    CHECK(vTaskDelete_fake.call_count == 1);
  }
  SECTION("Start")
  {
    constexpr uint32_t kPreRunSyncBits = 0xFFFF;
    xEventGroupCreateStatic_fake.custom_fake =
        xEventGroupCreateStatic_custom_fake;
    TaskScheduler scheduler         = TaskScheduler::Instance();
    constexpr uint8_t kMaxTaskCount = config::kTaskSchedulerSize;
    uint8_t task_count              = 0;
    for (uint8_t i = 0; i < kMaxTaskCount; i++)
    {
      scheduler.AddTask(&(mock_tasks[i].get()));
      task_count++;
    }
    CHECK(scheduler.GetTaskCount() == task_count);
    scheduler.Start();
    // xTaskCreateStatic and Setup should be invoked for each scheduled task
    Verify(Method(mock_tasks[0], Setup), Method(mock_tasks[1], Setup),
           Method(mock_tasks[2], Setup), Method(mock_tasks[3], Setup),
           Method(mock_tasks[4], Setup), Method(mock_tasks[5], Setup),
           Method(mock_tasks[6], Setup), Method(mock_tasks[7], Setup),
           Method(mock_tasks[8], Setup), Method(mock_tasks[9], Setup),
           Method(mock_tasks[10], Setup), Method(mock_tasks[11], Setup),
           Method(mock_tasks[12], Setup), Method(mock_tasks[13], Setup),
           Method(mock_tasks[14], Setup), Method(mock_tasks[15], Setup))
        .Exactly(1);
    CHECK(xTaskCreateStatic_fake.call_count == kMaxTaskCount);
    CHECK(xEventGroupCreateStatic_fake.call_count == 1);
    CHECK(scheduler.GetPreRunEventGroupHandle() == test_event_group_handle);
    CHECK(scheduler.GetPreRunSyncBits() == kPreRunSyncBits);
    CHECK(vTaskStartScheduler_fake.call_count == 1);
  }
}

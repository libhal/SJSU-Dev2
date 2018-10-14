// Tests for the TaskScheduler singleton class.
#include "L4_Application/task.hpp"
#include "L5_Testing/testing_frameworks.hpp"

EventGroupHandle_t test_event_group_handle;
EventGroupHandle_t xEventGroupCreateStatic_custom_fake(  // NOLINT
    StaticEventGroup_t *)
{
  return test_event_group_handle;
}

TEST_CASE("Testing TaskScheduler", "[task_scheduler]")
{
  using namespace rtos;  // NOLINT

  constexpr uint32_t kTotalTestTaskCount         = 2;
  const char * task_names[]                      = { "Task One", "Task Two" };
  TaskHandle_t task_handles[kTotalTestTaskCount] = { NULL };

  Mock<TaskInterface> mock_task_one;
  When(Method(mock_task_one, GetName)).AlwaysReturn(task_names[0]);
  When(Method(mock_task_one, Setup)).AlwaysReturn(true);
  When(Method(mock_task_one, Run)).AlwaysReturn(true);
  When(Method(mock_task_one, GetHandle)).AlwaysReturn(&task_handles[0]);
  When(Method(mock_task_one, GetStackSize)).AlwaysReturn(0);
  When(Method(mock_task_one, GetPriority)).AlwaysReturn(Priority::kLow);
  When(Method(mock_task_one, GetStack)).AlwaysReturn(0);
  When(Method(mock_task_one, GetTaskBuffer)).AlwaysReturn(0);

  Mock<TaskInterface> mock_task_two;
  When(Method(mock_task_two, GetName)).AlwaysReturn(task_names[1]);
  When(Method(mock_task_two, Setup)).AlwaysReturn(true);
  When(Method(mock_task_two, Run)).AlwaysReturn(true);
  When(Method(mock_task_two, GetHandle)).AlwaysReturn(&task_handles[1]);
  When(Method(mock_task_two, GetStackSize)).AlwaysReturn(0);
  When(Method(mock_task_two, GetPriority)).AlwaysReturn(Priority::kLow);
  When(Method(mock_task_two, GetStack)).AlwaysReturn(0);
  When(Method(mock_task_two, GetTaskBuffer)).AlwaysReturn(0);

  TaskInterface & task_one = mock_task_one.get();
  TaskInterface & task_two = mock_task_two.get();

  SECTION("AddTask")
  {
    TaskScheduler scheduler    = TaskScheduler::Instance();
    TaskInterface ** task_list = scheduler.GetAllTasks();
    scheduler.AddTask(&task_one);
    CHECK(scheduler.GetTaskCount() == 1);
    CHECK(!strcmp(task_list[0]->GetName(), task_names[0]));

    scheduler.AddTask(&task_two);
    CHECK(scheduler.GetTaskCount() == 2);
    CHECK(!strcmp(task_list[1]->GetName(), task_names[1]));
  }
  SECTION("GetTask")
  {
    TaskScheduler scheduler = TaskScheduler::Instance();
    scheduler.AddTask(&task_one);
    TaskInterface * fetched_task = scheduler.GetTask(task_names[0]);
    CHECK(!strcmp(fetched_task->GetName(), task_names[0]));
  }
  SECTION("GetTaskIndex")
  {
    TaskScheduler scheduler = TaskScheduler::Instance();
    scheduler.AddTask(&task_two);
    scheduler.AddTask(&task_one);
    CHECK(scheduler.GetTaskIndex(task_two.GetName()) == 0);
    CHECK(scheduler.GetTaskIndex(task_one.GetName()) == 1);
  }
  SECTION("RemoveTask")
  {
    TaskScheduler scheduler    = TaskScheduler::Instance();
    TaskInterface ** task_list = scheduler.GetAllTasks();
    scheduler.AddTask(&task_one);
    scheduler.AddTask(&task_two);
    // should do nothing since this taks is not scheduled
    scheduler.RemoveTask("Task A");
    CHECK(vTaskDelete_fake.call_count == 0);
    // Remove 'Task One' from scheduler
    // 'Task Two' should now be the first and only task in the scheduler
    scheduler.RemoveTask(task_names[0]);
    CHECK(scheduler.GetTaskCount() == 1);
    CHECK(task_list[0] == nullptr);
    CHECK(scheduler.GetTask(task_names[0]) == nullptr);
    // Remove 'Task Two' from scheduler
    // Scheduler should now be emtpy
    scheduler.RemoveTask(task_names[1]);
    CHECK(scheduler.GetTaskCount() == 0);
    CHECK(scheduler.GetTask(task_names[1]) == nullptr);
    CHECK(task_list[1] == nullptr);
    // should do nothing since there are no tasks
    scheduler.RemoveTask(task_names[0]);
    CHECK(vTaskDelete_fake.call_count == 2);
  }
  SECTION("Start")
  {
    constexpr uint32_t kPreRunSyncBits = (1 << 0) | (1 << 1);
    xEventGroupCreateStatic_fake.custom_fake =
        xEventGroupCreateStatic_custom_fake;
    TaskScheduler scheduler = TaskScheduler::Instance();
    // scheduler should initialize and start since there are tasks
    // scheduled
    scheduler.AddTask(&task_one);
    scheduler.AddTask(&task_two);
    CHECK(scheduler.GetTaskCount() == 2);
    scheduler.Start();
    Verify(Method(mock_task_one, Setup), Method(mock_task_two, Setup))
        .Exactly(1);
    CHECK(xTaskCreateStatic_fake.call_count == kTotalTestTaskCount);
    CHECK(xEventGroupCreateStatic_fake.call_count == 1);
    CHECK(scheduler.GetPreRunEventGroupHandle() == test_event_group_handle);
    CHECK(scheduler.GetPreRunSyncBits() == kPreRunSyncBits);
    CHECK(vTaskStartScheduler_fake.call_count == 1);
  }
}

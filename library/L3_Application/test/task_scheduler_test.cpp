// Tests for the TaskScheduler singleton class.
#include <iterator>

#include "L3_Application/task_scheduler.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace  // private namespace for custom fakes
{
EventGroupHandle_t test_event_group_handle;
EventGroupHandle_t xEventGroupCreateStatic_custom_fake(  // NOLINT
    StaticEventGroup_t *)
{
  return test_event_group_handle;
}
}  // namespace

namespace sjsu::rtos
{
TEST_CASE("Testing TaskScheduler")
{
  constexpr std::array kTaskNames = {
    "Task 1",  "Task 2",  "Task 3",  "Task 4",  "Task 5",  "Task 6",
    "Task 7",  "Task 8",  "Task 9",  "Task 10", "Task 11", "Task 12",
    "Task 13", "Task 14", "Task 15", "Task 16", "Task 17",
  };
  std::array<TaskHandle_t, kTaskNames.size()> task_handles;
  std::array<Mock<TaskInterface>, kTaskNames.size()> mock_tasks;
  for (size_t i = 0; i < kTaskNames.size(); i++)
  {
    INFO("Stubbing mock task: " << i);
    When(Method(mock_tasks[i], GetName)).AlwaysReturn(kTaskNames[i]);
    When(Method(mock_tasks[i], Setup)).AlwaysReturn(true);
    When(Method(mock_tasks[i], Run)).AlwaysReturn(true);
    When(Method(mock_tasks[i], GetHandle)).AlwaysReturn(&task_handles[i]);
    When(Method(mock_tasks[i], GetStackSize)).AlwaysReturn(0);
    When(Method(mock_tasks[i], GetPriority)).AlwaysReturn(Priority::kLow);
    When(Method(mock_tasks[i], GetStack)).AlwaysReturn(0);
    When(Method(mock_tasks[i], GetTaskBuffer)).AlwaysReturn(0);
  }

  /// The TaskScheduler object to test.
  TaskScheduler scheduler;

  SECTION("AddTask")
  {
    TaskInterface * const * task_list = scheduler.GetAllTasks();
    constexpr uint8_t kMaxTaskCount   = config::kTaskSchedulerSize;
    uint8_t task_count                = 0;

    // scheduler should be initially empty
    CHECK(scheduler.GetTaskCount() == 0);

    for (size_t i = 0; i < kMaxTaskCount; i++)
    {
      INFO("Testing AddTask() for task at index: " << i);
      // Setup
      TaskInterface & task = mock_tasks[i].get();

      // Exercise
      scheduler.AddTask(&task);
      task_count++;

      // Verify
      CHECK(scheduler.GetTaskCount() == task_count);
      CHECK(!strcmp(task_list[i]->GetName(), task.GetName()));
    }

    // scheduler should now be full and new tasks should not be added
    TaskInterface & task = mock_tasks[kMaxTaskCount].get();

    // Exercise
    scheduler.AddTask(&task);

    // Verify
    CHECK(scheduler.GetTaskCount() == task_count);
    CHECK(strcmp(task_list[kMaxTaskCount - 1]->GetName(), task.GetName()));
  }

  SECTION("GetTask")
  {
    constexpr uint8_t kMaxTaskCount = config::kTaskSchedulerSize;
    uint8_t task_count              = 0;
    CHECK(scheduler.GetTaskCount() == 0);

    SECTION("Getting a task that has been scheduled")
    {
      for (size_t i = 0; i < kMaxTaskCount; i++)
      {
        INFO("Testing GetTask() for task at index: " << i);

        // Setup
        TaskInterface & task = mock_tasks[i].get();
        scheduler.AddTask(&task);
        task_count++;

        // Exercise
        TaskInterface * retrieved_task = scheduler.GetTask(kTaskNames[i]);

        // Verify
        CHECK(scheduler.GetTaskCount() == task_count);
        CHECK(retrieved_task != nullptr);
        CHECK(!strcmp(retrieved_task->GetName(), task.GetName()));
      }
    }

    SECTION("Getting a task that has not been scheduled")
    {
      // Exercise
      TaskInterface * non_existent_task =
          scheduler.GetTask(kTaskNames[kMaxTaskCount]);

      // Verify
      CHECK(non_existent_task == nullptr);
    }
  }

  SECTION("GetTaskIndex")
  {
    constexpr uint8_t kMaxTaskCount = config::kTaskSchedulerSize;
    uint8_t task_count              = 0;

    SECTION("Getting the task indices of scheduled tasks")
    {
      for (size_t i = 0; i < kMaxTaskCount; i++)
      {
        INFO("Testing GetTaskIndex() for task at index: " << i);
        TaskInterface & task = mock_tasks[i].get();

        // Exercise
        scheduler.AddTask(&task);

        // Verify
        CHECK(scheduler.GetTaskIndex(task.GetName()) == task_count);
        task_count++;
        CHECK(scheduler.GetTaskCount() == task_count);
      }
    }

    SECTION("Getting the task index of a task that has not been scheduled")
    {
      // If retreiving the index of a task that is not scheduled, GetTaskIndex
      // should return kTaskSchedulerSize + 1
      CHECK(scheduler.GetTaskIndex("Does not exist") == (kMaxTaskCount + 1));
    }
  }

  SECTION("RemoveTask")
  {
    RESET_FAKE(vTaskDelete);

    SECTION("When scheduler is empty")
    {
      // Exercise
      scheduler.RemoveTask("Task A");

      // Verify: Should do nothing since "Task A" is not scheduled
      CHECK(vTaskDelete_fake.call_count == 0);
    }

    SECTION("When scheduler is not empty")
    {
      // Setup
      constexpr size_t kExpectedTaskCount  = 4;
      TaskInterface * const * task_list    = scheduler.GetAllTasks();
      constexpr uint8_t kTaskIndexToRemove = 2;

      for (size_t i = 0; i < kExpectedTaskCount; i++)
      {
        scheduler.AddTask(&(mock_tasks[i].get()));
      }
      CHECK(scheduler.GetTaskCount() == kExpectedTaskCount);

      // Exercise: Remove task named "Task 3"
      scheduler.RemoveTask(mock_tasks[kTaskIndexToRemove].get().GetName());

      // Verify
      CHECK(vTaskDelete_fake.call_count == 1);
      CHECK(scheduler.GetTaskCount() == (kExpectedTaskCount - 1));
      CHECK(task_list[kTaskIndexToRemove] == nullptr);
    }
  }

  SECTION("Start")
  {
    RESET_FAKE(xTaskCreateStatic);
    RESET_FAKE(xEventGroupCreateStatic);
    RESET_FAKE(vTaskStartScheduler);

    // Setup
    constexpr uint32_t kPreRunSyncBits = 0xFFFF;
    xEventGroupCreateStatic_fake.custom_fake =
        xEventGroupCreateStatic_custom_fake;
    constexpr uint8_t kMaxTaskCount = config::kTaskSchedulerSize;
    size_t task_count               = 0;
    // Add tasks 1-16 to the scheduler
    for (size_t i = 0; i < kMaxTaskCount; i++)
    {
      scheduler.AddTask(&(mock_tasks[i].get()));
      task_count++;
    }
    CHECK(scheduler.GetTaskCount() == task_count);

    // Exercise
    scheduler.Start();
    // xTaskCreateStatic and Setup should be invoked for each scheduled task
    for (size_t i = 0; i < task_count; i++)
    {
      INFO("Checking Setup invocation for task at index: " << i);
      Verify(Method(mock_tasks[i], Setup)).Once();
    }

    // Verify
    CHECK(xTaskCreateStatic_fake.call_count == kMaxTaskCount);
    CHECK(xEventGroupCreateStatic_fake.call_count == 1);
    CHECK(scheduler.GetPreRunEventGroupHandle() == test_event_group_handle);
    CHECK(scheduler.GetPreRunSyncBits() == kPreRunSyncBits);
    CHECK(vTaskStartScheduler_fake.call_count == 1);
  }
}
}  // namespace sjsu::rtos

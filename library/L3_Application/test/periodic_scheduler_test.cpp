// Tests for the PeriodicTask and PeriodicScheduler class.
#include "L3_Application/periodic_scheduler.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace  // private namespace for custom fakes
{
SemaphoreHandle_t test_semaphore;
SemaphoreHandle_t xQueueGenericCreateStatic_custom_fake(  // NOLINT
    UBaseType_t,
    UBaseType_t,
    uint8_t *,
    StaticQueue_t *,
    uint8_t)
{
  return test_semaphore;
}
}  // namespace

namespace sjsu::rtos
{
TEST_CASE("Testing PeriodicTask", "[periodic_task]")
{
  const char kTaskName[]                                 = "Test Periodic Task";
  constexpr Priority kTaskPriority                       = Priority::kLow;
  PeriodicTaskInterface::TaskFunction mock_task_function = [](uint32_t) {};

  Mock<TaskSchedulerInterface> mock_task_scheduler;
  Fake(Method(mock_task_scheduler, AddTask));

  RESET_FAKE(xQueueGenericCreateStatic);
  RESET_FAKE(xQueueSemaphoreTake);

  SECTION("Get periodic semaphore")
  {
    xQueueGenericCreateStatic_fake.custom_fake =
        xQueueGenericCreateStatic_custom_fake;
    PeriodicTask<512> test_task(kTaskName,
                                kTaskPriority,
                                &mock_task_function,
                                mock_task_scheduler.get());
    CHECK(xQueueGenericCreateStatic_fake.call_count == 1);
    CHECK(test_task.GetPeriodicSemaphore() == test_semaphore);
  }

  SECTION("Get task function")
  {
    PeriodicTask<512> test_task(kTaskName,
                                kTaskPriority,
                                &mock_task_function,
                                mock_task_scheduler.get());
    CHECK(test_task.GetTaskFunction() == (&mock_task_function));
  }
}

TEST_CASE("Testing PeriodicScheduler", "[periodic_scheduler]")
{
  RESET_FAKE(xTimerCreateStatic);
  RESET_FAKE(xTimerGenericCommand);

  const char kPeriodicSchedulerName[] = "Periodic Scheduler";
  Mock<TaskSchedulerInterface> mock_task_scheduler;
  Fake(Method(mock_task_scheduler, AddTask));

  SECTION("Initialization")
  {
    PeriodicScheduler scheduler =
        PeriodicScheduler(kPeriodicSchedulerName, mock_task_scheduler.get());
    CHECK(scheduler.GetPriority() == Priority::kLow);
    // A total of four timers should be created for each of the task frequencies
    // with each timer set to its active state
    constexpr uint8_t kExpectedTimerCount = PeriodicScheduler::kMaxTaskCount;
    CHECK(xTimerCreateStatic_fake.call_count == kExpectedTimerCount);
    CHECK(xTimerGenericCommand_fake.call_count == kExpectedTimerCount);
  }

  SECTION("GetTask")
  {
    PeriodicScheduler scheduler =
        PeriodicScheduler(kPeriodicSchedulerName, mock_task_scheduler.get());

    SECTION("Getting a task that was not scheduled")
    {
      for (size_t i = 0; i < PeriodicScheduler::kMaxTaskCount; i++)
      {
        PeriodicScheduler::Frequency frequency =
            PeriodicScheduler::Frequency(i);
        CHECK(scheduler.GetTask(frequency) == nullptr);
      }
    }

    SECTION("Getting a scheduled task")
    {
      RESET_FAKE(vTimerSetTimerID);

      std::array<PeriodicTaskInterface::TaskFunction,
                 PeriodicScheduler::kMaxTaskCount>
          mock_task_functions = {
            [](uint32_t) {},
            [](uint32_t) {},
            [](uint32_t) {},
            [](uint32_t) {},
          };
      // Stub PeriodicTask mocks with task functions.
      Mock<PeriodicTaskInterface> mock_tasks[mock_task_functions.size()];
      for (size_t i = 0; i < mock_task_functions.size(); i++)
      {
        When(Method(mock_tasks[i], GetTaskFunction))
            .AlwaysReturn(&(mock_task_functions[i]));
      }

      for (size_t i = 0; i < mock_task_functions.size(); i++)
      {
        INFO("Testing set/get task function for task at index: " << i);
        PeriodicScheduler::Frequency frequency =
            PeriodicScheduler::Frequency(i);
        PeriodicTaskInterface * task = &(mock_tasks[i].get());

        scheduler.SetTask(task, frequency);
        PeriodicTaskInterface * retrieved_task = scheduler.GetTask(frequency);
        CHECK(retrieved_task == task);
        CHECK(retrieved_task->GetTaskFunction() == &(mock_task_functions[i]));
        CHECK(vTimerSetTimerID_fake.call_count == (i + 1));
      }
    }
  }
}
}  // namespace sjsu::rtos

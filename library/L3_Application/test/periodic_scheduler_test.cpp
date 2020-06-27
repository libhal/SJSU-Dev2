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
TEST_CASE("Testing PeriodicTask")
{
  const char kTaskName[]                                 = "Test Periodic Task";
  constexpr Priority kTaskPriority                       = Priority::kLow;
  PeriodicTaskInterface::TaskFunction mock_task_function = [](uint32_t) {};

  RESET_FAKE(xQueueGenericCreateStatic);
  RESET_FAKE(xQueueSemaphoreTake);

  SECTION("Get periodic semaphore")
  {
    // Setup
    xQueueGenericCreateStatic_fake.custom_fake =
        xQueueGenericCreateStatic_custom_fake;
    PeriodicTask<512> test_task(kTaskName, kTaskPriority, &mock_task_function);

    // Exercise
    auto actual_result = test_task.GetPeriodicSemaphore();

    // Verify
    CHECK(xQueueGenericCreateStatic_fake.call_count == 1);
    CHECK(actual_result == test_semaphore);
  }

  SECTION("Get task function")
  {
    // Setup
    PeriodicTask<512> test_task(kTaskName, kTaskPriority, &mock_task_function);

    // Exercise
    auto actual_result = test_task.GetTaskFunction();

    // Verify
    CHECK(actual_result == (&mock_task_function));
  }
}

TEST_CASE("Testing PeriodicScheduler")
{
  RESET_FAKE(xTimerCreateStatic);
  RESET_FAKE(xTimerGenericCommand);

  const char kPeriodicSchedulerName[] = "Periodic Scheduler";
  PeriodicScheduler scheduler = PeriodicScheduler(kPeriodicSchedulerName);

  SECTION("Initialization")
  {
    // A total of four timers should be created for each of the task frequencies
    // with each timer set to its active state
    constexpr uint8_t kExpectedTimerCount = PeriodicScheduler::kMaxTaskCount;

    // Verify
    CHECK(scheduler.GetPriority() == Priority::kLow);
    CHECK(xTimerCreateStatic_fake.call_count == kExpectedTimerCount);
    CHECK(xTimerGenericCommand_fake.call_count == kExpectedTimerCount);
  }

  SECTION("GetTask")
  {
    SECTION("Getting a task that was not scheduled")
    {
      for (size_t i = 0; i < PeriodicScheduler::kMaxTaskCount; i++)
      {
        // Setup
        PeriodicScheduler::Frequency frequency =
            PeriodicScheduler::Frequency(i);

        // Exercise
        auto actual_result = scheduler.GetTask(frequency);

        // Verify
        CHECK(actual_result == nullptr);
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
        // Setup
        INFO("Testing set/get task function for task at index: " << i);
        PeriodicScheduler::Frequency frequency =
            PeriodicScheduler::Frequency(i);
        PeriodicTaskInterface * task = &(mock_tasks[i].get());

        // Exercise
        scheduler.SetTask(task, frequency);
        PeriodicTaskInterface * retrieved_task = scheduler.GetTask(frequency);

        // Verify
        CHECK(retrieved_task == task);
        CHECK(retrieved_task->GetTaskFunction() == &(mock_task_functions[i]));
        CHECK(vTimerSetTimerID_fake.call_count == (i + 1));
      }
    }
  }
}
}  // namespace sjsu::rtos

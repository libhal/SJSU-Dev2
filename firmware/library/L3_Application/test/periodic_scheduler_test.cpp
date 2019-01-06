// Tests for the PeriodicTask and PeriodicScheduler class.
#include "L3_Application/periodic_scheduler.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace
{
SemaphoreHandle_t test_semaphore;
SemaphoreHandle_t xQueueGenericCreateStatic_custom_fake(  // NOLINT
    UBaseType_t, UBaseType_t, uint8_t *, StaticQueue_t *, uint8_t)
{
  return test_semaphore;
}

FAKE_VOID_FUNC(PeriodicTaskTestFunction1Hz, uint32_t);
FAKE_VOID_FUNC(PeriodicTaskTestFunction10Hz, uint32_t);
FAKE_VOID_FUNC(PeriodicTaskTestFunction100Hz, uint32_t);
FAKE_VOID_FUNC(PeriodicTaskTestFunction1000Hz, uint32_t);
}  // namespace

TEST_CASE("Testing PeriodicTask", "[periodic_task]")
{
  using namespace rtos;  // NOLINT

  const char kTaskName[]           = "Test Periodic Task";
  constexpr Priority kTaskPriority = Priority::kLow;

  RESET_FAKE(xQueueGenericCreateStatic);
  RESET_FAKE(xQueueSemaphoreTake);

  SECTION("Get periodic semaphore")
  {
    xQueueGenericCreateStatic_fake.custom_fake =
        xQueueGenericCreateStatic_custom_fake;
    PeriodicTask<512> test_task(kTaskName, kTaskPriority,
                                PeriodicTaskTestFunction1Hz);
    CHECK(xQueueGenericCreateStatic_fake.call_count == 1);
    CHECK(test_task.GetPeriodicSemaphore() == test_semaphore);
  }
  SECTION("Get task function")
  {
    PeriodicTask<512> test_task(kTaskName, kTaskPriority,
                                PeriodicTaskTestFunction1Hz);
    CHECK(test_task.GetTaskFunction() == PeriodicTaskTestFunction1Hz);
  }
}

TEST_CASE("Testing PeriodicScheduler", "[periodic_scheduler]")
{
  using namespace rtos;  // NOLINT

  constexpr uint8_t kMaxTaskCount = PeriodicScheduler::kMaxTaskCount;
  constexpr PeriodicTaskFunction kTaskFunctions[] = {
    PeriodicTaskTestFunction1Hz, PeriodicTaskTestFunction10Hz,
    PeriodicTaskTestFunction100Hz, PeriodicTaskTestFunction1000Hz
  };
  Mock<PeriodicTaskInterface> mock_tasks[kMaxTaskCount];
  for (uint8_t i = 0; i < kMaxTaskCount; i++)
  {
    When(Method(mock_tasks[i], GetPeriodicSemaphore))
        .AlwaysReturn(test_semaphore);
    When(Method(mock_tasks[i], GetTaskFunction))
        .AlwaysReturn(kTaskFunctions[i]);
  }

  SECTION("Initialization")
  {
    PeriodicScheduler scheduler = PeriodicScheduler();
    CHECK(scheduler.GetPriority() == Priority::kHigh);
    CHECK(scheduler.GetDelayTime() == 1);
  }
  SECTION("Setting and getting a task function")
  {
    PeriodicScheduler scheduler = PeriodicScheduler();
    for (uint8_t i = 0; i < kMaxTaskCount; i++)
    {
      PeriodicTaskInterface * task           = &(mock_tasks[i].get());
      PeriodicScheduler::Frequency frequency = PeriodicScheduler::Frequency(i);
      scheduler.SetTask(task, frequency);
      CHECK(scheduler.GetTask(frequency) == task);
    }
  }
}

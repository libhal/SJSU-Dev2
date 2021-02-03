#define DOCTEST_CONFIG_IMPLEMENT

#include <unistd.h>

#include <span>

#include "platforms/utility/ram.hpp"
#include "newlib/newlib.hpp"
#include "testing_frameworks.hpp"
#include "utility/rtos/freertos/rtos.hpp"

DEFINE_FFF_GLOBALS

DEFINE_FAKE_VOID_FUNC(vTaskStartScheduler);
DEFINE_FAKE_VOID_FUNC(vTaskSuspend, TaskHandle_t);
DEFINE_FAKE_VOID_FUNC(vTaskResume, TaskHandle_t);
DEFINE_FAKE_VOID_FUNC(vTaskDelete, TaskHandle_t);
DEFINE_FAKE_VOID_FUNC(vTaskDelayUntil, TickType_t *, TickType_t);
DEFINE_FAKE_VOID_FUNC(vApplicationGetIdleTaskMemory,
                      StaticTask_t **,
                      StackType_t **,
                      uint32_t *);

DEFINE_FAKE_VALUE_FUNC(TickType_t, xTaskGetTickCount);
DEFINE_FAKE_VALUE_FUNC(TaskHandle_t,
                       xTaskCreateStatic,
                       TaskFunction_t,
                       const char *,
                       uint32_t,
                       void *,
                       UBaseType_t,
                       StackType_t *,
                       StaticTask_t *);

DEFINE_FAKE_VALUE_FUNC(EventGroupHandle_t,
                       xEventGroupCreateStatic,
                       StaticEventGroup_t *);
DEFINE_FAKE_VALUE_FUNC(EventBits_t,
                       xEventGroupSync,
                       EventGroupHandle_t,
                       EventBits_t,
                       EventBits_t,
                       TickType_t);

DEFINE_FAKE_VALUE_FUNC(QueueHandle_t,
                       xQueueGenericCreateStatic,
                       const UBaseType_t,
                       const UBaseType_t,
                       uint8_t *,
                       StaticQueue_t *,
                       const uint8_t);
DEFINE_FAKE_VALUE_FUNC(BaseType_t,
                       xQueueGenericSend,
                       QueueHandle_t,
                       const void *,
                       TickType_t,
                       BaseType_t);
DEFINE_FAKE_VALUE_FUNC(BaseType_t,
                       xQueueSemaphoreTake,
                       QueueHandle_t,
                       TickType_t);

DEFINE_FAKE_VALUE_FUNC(TimerHandle_t,
                       xTimerCreateStatic,
                       const char *,
                       TickType_t,
                       UBaseType_t,
                       void *,
                       TimerCallbackFunction_t,
                       StaticTimer_t *);
DEFINE_FAKE_VALUE_FUNC(BaseType_t,
                       xTimerGenericCommand,
                       TimerHandle_t,
                       BaseType_t,
                       TickType_t,
                       BaseType_t *,
                       TickType_t);
DEFINE_FAKE_VALUE_FUNC(void *, pvTimerGetTimerID, TimerHandle_t);
DEFINE_FAKE_VOID_FUNC(vTimerSetTimerID, TimerHandle_t, void *);

// =============================================================================
// Define Empty InitializePlatform()
// =============================================================================

namespace sjsu
{
void InitializePlatform() {}
}  // namespace sjsu

// =============================================================================
// Ram definitions for testing
// =============================================================================

namespace
{
struct DataSection_t
{
  int32_t a;
  uint8_t b;
  double d;
  uint16_t s;
};

DataSection_t rom = {
  .a = 15,
  .b = 'C',
  .d = 5.0,
  .s = 12'346U,
};

DataSection_t ram;

std::array<uint32_t, 128> bss_section;
}  // namespace

DataSectionTable_t data_section_table[] = {
  DataSectionTable_t{
      .rom_location = reinterpret_cast<uint32_t *>(&rom),
      .ram_location = reinterpret_cast<uint32_t *>(&ram),
      .length       = sizeof(rom),
  },
};

// NOTE: Not used, but must be defined
DataSectionTable_t data_section_table_end;

BssSectionTable_t bss_section_table[] = {
  BssSectionTable_t{
      .ram_location = bss_section.data(),
      .length       = bss_section.size(),
  },
};

// NOTE: Not used, but must be defined
BssSectionTable_t bss_section_table_end;

// =============================================================================
// Setup write() and read()
// =============================================================================

int HostTestWrite(std::span<const char> str)
{
  return static_cast<int>(write(1, str.data(), str.size()));
}

int HostTestRead(std::span<char> str)
{
  return static_cast<int>(read(1, str.data(), str.size()));
}

int main(int argc, char * argv[])
{
  doctest::Context context;

  sjsu::newlib::SetStdout(HostTestWrite);
  sjsu::newlib::SetStdin(HostTestRead);

  context.applyCommandLine(argc, argv);

  int res = context.run();  // run

  // important - query flags (and --exit) rely on the user doing this propagate
  // the result of the tests
  if (context.shouldExit())
  {
    return res;
  }

  int client_stuff_return_code = 0;
  // your program - if the testing framework is integrated in your production
  // code

  return res + client_stuff_return_code;  // the result from doctest is
                                          // propagated here as well
}

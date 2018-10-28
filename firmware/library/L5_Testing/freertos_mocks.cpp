#include "L2_Utilities/rtos.hpp"

#if defined HOST_TEST
DEFINE_FAKE_VOID_FUNC(vTaskStartScheduler);
DEFINE_FAKE_VOID_FUNC(vTaskSuspend, TaskHandle_t);
DEFINE_FAKE_VOID_FUNC(vTaskResume, TaskHandle_t);
DEFINE_FAKE_VOID_FUNC(vTaskDelete, TaskHandle_t);
DEFINE_FAKE_VOID_FUNC(vTaskDelayUntil, TickType_t *, TickType_t);
DEFINE_FAKE_VOID_FUNC(vApplicationGetIdleTaskMemory, StaticTask_t **,
                      StackType_t **, uint32_t *);
DEFINE_FAKE_VALUE_FUNC(TickType_t, xTaskGetTickCount);
DEFINE_FAKE_VALUE_FUNC(TaskHandle_t, xTaskCreateStatic, TaskFunction_t,
                       const char *, uint32_t, void *, UBaseType_t,
                       StackType_t *, StaticTask_t *);
DEFINE_FAKE_VALUE_FUNC(EventGroupHandle_t, xEventGroupCreateStatic,
                       StaticEventGroup_t *);
DEFINE_FAKE_VALUE_FUNC(EventBits_t, xEventGroupSync, EventGroupHandle_t,
                       EventBits_t, EventBits_t, TickType_t);
#endif  // HOST_TEST

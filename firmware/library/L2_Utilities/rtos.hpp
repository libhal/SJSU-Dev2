// This file contains helper and utility functions, objects and types for
// FreeRTOS.
#pragma once

#include <cstdint>

#include "FreeRTOS.h"
#include "task.h"

#if defined HOST_TEST
#include "event_groups.h"
#include "L5_Testing/testing_frameworks.hpp"

DECLARE_FAKE_VOID_FUNC(vTaskStartScheduler);
DECLARE_FAKE_VOID_FUNC(vTaskSuspend, TaskHandle_t);
DECLARE_FAKE_VOID_FUNC(vTaskResume, TaskHandle_t);
DECLARE_FAKE_VOID_FUNC(vTaskDelete, TaskHandle_t);
DECLARE_FAKE_VOID_FUNC(vTaskDelayUntil, TickType_t *, TickType_t);
DECLARE_FAKE_VOID_FUNC(vApplicationGetIdleTaskMemory, StaticTask_t **,
                       StackType_t **, uint32_t *);
DECLARE_FAKE_VALUE_FUNC(TickType_t, xTaskGetTickCount);
DECLARE_FAKE_VALUE_FUNC(TaskHandle_t, xTaskCreateStatic, TaskFunction_t,
                        const char *, uint32_t, void *, UBaseType_t,
                        StackType_t *, StaticTask_t *);
DECLARE_FAKE_VALUE_FUNC(EventGroupHandle_t, xEventGroupCreateStatic,
                        StaticEventGroup_t *);
DECLARE_FAKE_VALUE_FUNC(EventBits_t, xEventGroupSync, EventGroupHandle_t,
                        EventBits_t, EventBits_t, TickType_t);
#endif  // HOST_TEST

namespace rtos
{
enum Priority
{
  kIdle = tskIDLE_PRIORITY,
  kLow,
  kMedium,
  kHigh,
  kCritical
};

constexpr void * kNoParameter = nullptr;
constexpr void ** kNoHandle   = nullptr;

// Calculates and returns the stack size of a task. This will add in the minimum
// needed stack size for a task.
constexpr size_t StackSize(size_t stack_size_bytes)
{
  return configMINIMAL_STACK_SIZE + (stack_size_bytes / sizeof(StackType_t));
}
// Allows the developer to convert primitive type parameter to an rtos task by
// converting it into an void*. The size of this parameter must be equal to or
// smaller than intptr_t, otherwise it will not fit. If this is the case,
// passing such a parameter must be pasted by pointer.
template <typename T>
constexpr void * PassParameter(T t)
{
  static_assert(sizeof(T) <= sizeof(intptr_t),
                "The size of the type must be, smaller than or equalt to the "
                "size of a pointer.");
  return reinterpret_cast<void *>(t);
}
// Convert pointer to an integer type. The result of this can then be cast to
// other types.
inline intptr_t RetrieveParameter(void * parameter)
{
  return reinterpret_cast<intptr_t>(parameter);
}

}  // namespace rtos

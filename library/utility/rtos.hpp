// This file contains helper and utility functions, objects and types for
// FreeRTOS.
#pragma once

#include <FreeRTOS.h>

#include <cstdint>

#include "event_groups.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"

namespace sjsu::rtos
{
enum Priority
{
  kIdle = 0,
  kLow,
  kMedium,
  kHigh,
  kCritical
};

constexpr void * kNoParameter = nullptr;
constexpr void ** kNoHandle   = nullptr;

/// Calculates and returns the stack size of a task. This will add in the
/// minimum needed stack size for a task.
///
/// @param stack_size_bytes - The number of bytes you want your task's stack to
///        occupy.
/// @return the number of stack elements (StackType_t) that are needed to reach
///         at least the desired stack_size_bytes, aligned to the size of
///         StackType_t.
constexpr size_t StackSize(size_t stack_size_bytes)
{
  return configMINIMAL_STACK_SIZE + (stack_size_bytes / sizeof(StackType_t));
}
/// Allows the developer to convert primitive type parameter to an sjsu::rtos
/// task by converting it into an void*. The size of this parameter must be
/// equal to or smaller than intptr_t, otherwise it will not fit. If this is the
/// case, passing such a parameter must be pasted by pointer.
///
/// @tparam T - Type of the passed parameter.
/// @param value - The value of the passed parameter
/// @return the value as a void*.
template <typename T>
constexpr void * PassParameter(T value)
{
  static_assert(sizeof(T) <= sizeof(intptr_t),
                "The size of the type must be, smaller than or equal to the "
                "size of a pointer.");
  return reinterpret_cast<void *>(value);
}
/// Convert pointer to an integer type. The result of this can then be cast to
/// other types.
///
/// @param parameter - the void* to convert to an intptr_t.
/// @return convert parameter to an intptr_t.
inline intptr_t RetrieveParameter(void * parameter)
{
  return reinterpret_cast<intptr_t>(parameter);
}
}  // namespace sjsu::rtos

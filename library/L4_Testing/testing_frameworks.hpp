#pragma once

#include <cinttypes>
#include <span>
#include <string>
#include <utility>

#include "third_party/doctest/doctest/doctest.h"

// Quick and dirty way to have sub sections from Catch2 translate to SUBCASE in
// doctest.
#undef SUBCASE
#define SUBCASE(...) DOCTEST_SUBCASE(std::string(__VA_ARGS__).c_str())
#define SECTION SUBCASE

template <typename T>
bool operator==(std::span<T> lhs, std::span<T> rhs)
{
  if (lhs.size() != rhs.size())
  {
    return false;
  }

  return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

#include "third_party/fakeit/fakeit.hpp"
#include "third_party/fff/fff.h"
#include "utility/error_handling.hpp"
#include "utility/rtos.hpp"

// =============================================================================
// Fake FreeRTOS Functions
// =============================================================================

DECLARE_FAKE_VOID_FUNC(vTaskStartScheduler);
DECLARE_FAKE_VOID_FUNC(vTaskSuspend, TaskHandle_t);
DECLARE_FAKE_VOID_FUNC(vTaskResume, TaskHandle_t);
DECLARE_FAKE_VOID_FUNC(vTaskDelete, TaskHandle_t);
DECLARE_FAKE_VOID_FUNC(vTaskDelayUntil, TickType_t *, TickType_t);
DECLARE_FAKE_VOID_FUNC(vApplicationGetIdleTaskMemory,
                       StaticTask_t **,
                       StackType_t **,
                       uint32_t *);

DECLARE_FAKE_VALUE_FUNC(TickType_t, xTaskGetTickCount);
DECLARE_FAKE_VALUE_FUNC(TaskHandle_t,
                        xTaskCreateStatic,
                        TaskFunction_t,
                        const char *,
                        uint32_t,
                        void *,
                        UBaseType_t,
                        StackType_t *,
                        StaticTask_t *);

DECLARE_FAKE_VALUE_FUNC(EventGroupHandle_t,
                        xEventGroupCreateStatic,
                        StaticEventGroup_t *);
DECLARE_FAKE_VALUE_FUNC(EventBits_t,
                        xEventGroupSync,
                        EventGroupHandle_t,
                        EventBits_t,
                        EventBits_t,
                        TickType_t);

DECLARE_FAKE_VALUE_FUNC(QueueHandle_t,
                        xQueueGenericCreateStatic,
                        UBaseType_t,
                        UBaseType_t,
                        uint8_t *,
                        StaticQueue_t *,
                        uint8_t);
DECLARE_FAKE_VALUE_FUNC(BaseType_t,
                        xQueueGenericSend,
                        QueueHandle_t,
                        const void *,
                        TickType_t,
                        BaseType_t);
DECLARE_FAKE_VALUE_FUNC(BaseType_t,
                        xQueueSemaphoreTake,
                        QueueHandle_t,
                        TickType_t);

DECLARE_FAKE_VALUE_FUNC(TimerHandle_t,
                        xTimerCreateStatic,
                        const char *,
                        TickType_t,
                        UBaseType_t,
                        void *,
                        TimerCallbackFunction_t,
                        StaticTimer_t *);
DECLARE_FAKE_VALUE_FUNC(BaseType_t,
                        xTimerGenericCommand,
                        TimerHandle_t,
                        BaseType_t,
                        TickType_t,
                        BaseType_t *,
                        TickType_t);
DECLARE_FAKE_VALUE_FUNC(void *, pvTimerGetTimerID, TimerHandle_t);
DECLARE_FAKE_VOID_FUNC(vTimerSetTimerID, TimerHandle_t, void *);

using namespace fakeit;  // NOLINT

int HostWrite(std::span<const char> payload);
int HostRead(std::span<char> payload);

/// This magical method does a lot of tricks to convince the compiler to emit
/// as many of the class methods as possible for methods of a function that may
/// only exist in a header file.
#define EMIT_ALL_METHODS(class_name)                         \
  namespace                                                  \
  {                                                          \
  template <unsigned test_parameter>                         \
  using class_name##_Template = class_name;                  \
  int __LetsGo_##class_name(class_name##_Template<0> * obj)  \
  {                                                          \
    bool never_true = false;                                 \
    bool * ptr      = &never_true;                           \
    /* Emit destructors */                                   \
    if (*ptr)                                                \
    {                                                        \
      class_name##_Template<0> emitter = *obj;               \
      (void)emitter;                                         \
      return 1;                                              \
    }                                                        \
    return 0;                                                \
  }                                                          \
  auto __full_##class_name = __LetsGo_##class_name(nullptr); \
  }  // namespace

/// @param expression - and expression that, when executed will throw
/// @param error_code - the error code to compare the one held by the exception
#define SJ2_CHECK_EXCEPTION(expression, error_code)                        \
  try                                                                      \
  {                                                                        \
    /* Execute expression */                                               \
    (expression);                                                          \
    /* Fail this check if this area is reached. */                         \
    CHECK_MESSAGE(false, "Exception was NOT thrown when it should have!"); \
  }                                                                        \
  catch (const ::sjsu::Exception & e)                                      \
  {                                                                        \
    /* Verify */                                                           \
    CHECK(e.GetCode() == error_code);                                      \
  }

template <typename T>
class Reflection
{
 public:
  static constexpr const char * Name()
  {
    return "?";
  }
};

template <>
class Reflection<uint8_t>
{
 public:
  static constexpr const char * Name()
  {
    return "uint8_t";
  }
};

template <>
class Reflection<const uint8_t>
{
 public:
  static constexpr const char * Name()
  {
    return "const uint8_t";
  }
};

template <>
class Reflection<uint16_t>
{
 public:
  static constexpr const char * Name()
  {
    return "uint16_t";
  }
};

template <>
class Reflection<uint32_t>
{
 public:
  static constexpr const char * Name()
  {
    return "uint32_t";
  }
};

template <>
class Reflection<uint64_t>
{
 public:
  static constexpr const char * Name()
  {
    return "uint64_t";
  }
};

template <>
class Reflection<int8_t>
{
 public:
  static constexpr const char * Name()
  {
    return "int8_t";
  }
};

template <>
class Reflection<int16_t>
{
 public:
  static constexpr const char * Name()
  {
    return "int16_t";
  }
};

template <>
class Reflection<int32_t>
{
 public:
  static constexpr const char * Name()
  {
    return "int32_t";
  }
};

template <>
class Reflection<int64_t>
{
 public:
  static constexpr const char * Name()
  {
    return "int64_t";
  }
};

namespace doctest
{
template <typename T, size_t N>
struct StringMaker<std::array<T, N>>  // NOLINT
{
  static String convert(const std::array<T, N> & array)  // NOLINT
  {
    std::string str;

    str += "std::array<";
    str += Reflection<T>::Name();
    str += ", ";
    str += std::to_string(N);
    str += ">{";

    size_t i = 0;
    for (i = 0; i < N - 1; i++)
    {
      str += std::to_string(array[i]) + ", ";
    }

    str += std::to_string(array[i]);

    str += " }";

    String result(str.data(), str.size());
    return result;
  }
};

template <typename T>
struct StringMaker<std::span<T>>
{
  static String convert(const std::span<T> & span)  // NOLINT
  {
    std::string str;

    str += "std::span<";
    str += Reflection<T>::Name();
    str += ", ";
    str += std::to_string(span.size());
    str += ">{ ";

    size_t i = 0;
    for (i = 0; i < span.size() - 1; i++)
    {
      str += std::to_string(span[i]) + ", ";
    }

    str += std::to_string(span[i]);

    str += " }";

    String result(str.data(), str.size());
    return result;
  }
};

template <>
struct StringMaker<std::errc>
{
  static String convert(const std::errc & error_code)  // NOLINT
  {
    return sjsu::Stringify(error_code);
  }
};

template <typename T, typename U>
struct StringMaker<std::chrono::duration<T, U>>  // NOLINT
{
  static String convert(const std::chrono::duration<T, U> & duration)  // NOLINT
  {
    return std::to_string(duration.count()).c_str();
  }
};
}  // namespace doctest

REGISTER_EXCEPTION_TRANSLATOR(sjsu::Exception & e)
{
  return doctest::String(e.what());
}

namespace sjsu::testing
{
/// Best practice way to clear a structure of its contents to all zeros for
/// testing. Test developers should not call memset directly in their code. This
/// will handle deducing the size of the structure which an lead to problems if
/// there is a typo.
///
/// @tparam T - type of the structure (used to deduce the size of the type)
/// @param data_structure - data structure to set to all zeros.
template <class T>
inline void ClearStructure(T * data_structure)
{
  memset(data_structure, 0, sizeof(*data_structure));
}
}  // namespace sjsu::testing

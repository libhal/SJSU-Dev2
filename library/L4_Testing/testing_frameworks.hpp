#pragma once

#include <cinttypes>
#include <functional>
#include <span>
#include <string>
#include <thread>
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

int HostTestWrite(std::span<const char> payload);
int HostTestRead(std::span<char> payload);

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

/// Poll verification parameter to condense the arguments of the
/// PollingVerification function.
struct PollVerificationParameter_t
{
  /// Locking function must setup the environment in such a way to cause the
  /// polling_function to lock up in a loop and wait for an event to occur to
  /// release it. For example, polling_function() will wait for Register A to be
  /// 10 before finishing. The locking function will need to set Register A to
  /// something other than 10, lets say 0, in order to cause the polling
  /// function to poll.
  std::function<void(void)> locking_function;

  /// A function that poll when called. This function should wait for an event
  /// to occur like a register update.
  std::function<void(void)> polling_function;

  /// Release function must configure the environment in such a way to cause the
  /// polling function to no longer poll. This function can be used to verify
  /// that the environment has been configured properly by the polling_function.
  /// For example, if you know that the polling function will set a register (A)
  /// to the value 5 before it polls on register B for the value 10, the release
  /// function can check if register A is 5, then the release function can set
  /// register B to 10.
  std::function<void(void)> release_function;

  /// Delay time between calling the polling function and running the release
  /// function. This is also the delay time between calling the release function
  /// and verifying that the polling function was released by the release
  /// function. Default of 1ms should be more than enough for most use cases.
  std::chrono::nanoseconds delay_time = 5ms;
};

/// A utility function to help scaffold and generalize test code that has polls
/// on a specific condition. Will fail test case and abort all sub-threads if a
/// failure occurs. This function is safe to use and shall halt the program any
/// more than 2x the time of delay_time.
///
/// USE this function rather than using std::thread directly to verify the state
/// of a system when it begins to poll.
///
/// @param poll - see documentation for this type for more details about each
///        field and how they should be used.
/// @param location - used to print out the location in the test where the
///        failure occurred. Do not
inline void PollingVerification(
    PollVerificationParameter_t poll,
    const std::experimental::source_location & location =
        std::experimental::source_location::current())
{
  // Setup
  // Setup: Create a polling variable that will track if the polling function is
  //        still polling or not. This is set to false after the polling
  //        function is finished. This variable only become false the polling
  //        function was never locked in a loop or if environmental change due
  //        to the release function results in the polling_function finishing.
  bool polling = true;

  // Setup: First run the function that will setup the environment to cause the
  //        polling function to poll/loop/lock.
  poll.locking_function();

  std::string file_location =
      location.file_name() + std::string(":") + std::to_string(location.line());

  std::string lock_failure_string = file_location;
  lock_failure_string +=
      ": locking_function() did not result in the polling_function() locking. "
      "Failing this test!";

  std::string release_failure_string = file_location;
  release_failure_string +=
      ": polling_function() was not released by release_function(). Failing "
      "this test!";

  // Exercise
  // Exercise: Create the additional thread that will release the
  //           polling_function() from polling after delay_time has elapsed.
  //           Will also verify that that polling variable is correct.
  std::thread polling_release_thread(
      [&poll, &polling, &lock_failure_string, &release_failure_string]() {
        // Wait for polling function to begin polling
        std::this_thread::sleep_for(poll.delay_time);

        // Check that polling variable is still TRUE (will become false if
        // polling finished early).
        REQUIRE_MESSAGE(polling, lock_failure_string.c_str());

        // Attempt to release the polling function from polling.
        poll.release_function();

        // Wait for polling function to finish.
        std::this_thread::sleep_for(poll.delay_time);

        // Check that polling variable is now FALSE
        REQUIRE_MESSAGE(!polling, release_failure_string.c_str());
      });

  // Exercise: Begin polling
  poll.polling_function();

  // Polling function has finished at this point, set polling to false.
  polling = false;

  // Wait for polling thread to finish.
  polling_release_thread.join();
}
}  // namespace sjsu::testing

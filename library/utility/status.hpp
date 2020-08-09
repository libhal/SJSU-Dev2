/// For information on how to use this library please see:
///
///     demos/multiplatform/status
///
#pragma once

#include <cinttypes>
#include <limits>
#include <string_view>

#if defined(__clang_analyzer__)
#include "utility/dummy/source_location"
#else
#include <experimental/source_location>
#endif

#include "utility/debug.hpp"
#include "third_party/expected/include/tl/expected.hpp"

namespace sjsu
{
/// Parent structure of the `Status` object containing the status code as well
/// as the status' name.
struct Status_t
{
  /// The status code number
  int code = std::numeric_limits<int>::max();

  /// The string representation of the status.
  std::string_view name = "Unknown";

  /// Allows explicit static_cast<> conversion from Status_t to int
  /// @return int - the error code as this type
  explicit operator int() const
  {
    return code;
  }
};

/// Helper factory function for creating a Status_t at compile time. If you
/// want to create your own status types you can do so by using this function
/// and supplying your own status_code and status_name. It is recommend to use
/// positive numbers for user supplied status' as the SJSU-Dev2's status's are
/// all negative.
///
/// @param status_code - Number associated with the status code.
/// @param status_name - String to represent the name of the status.
/// @return constexpr Status_t - resulting Status_t
constexpr Status_t CreateStatus(int status_code, std::string_view status_name)
{
  return Status_t{ .code = status_code, .name = status_name };
}

/// A status object that indicates the type of error or exception that has
/// occurred within the code. This object is also a container/namespace for
/// the standard set of status's defined for SJSU-Dev2.
struct Status : public Status_t  // NOLINT
{
  /// Allows the creation of a default Status object
  constexpr Status() {}

  /// Factory object that is used to simplify the creation of status objects.
  ///
  /// @param status_code - Number associated with the status code.
  /// @param status_name - String to represent the name of the status.
  constexpr Status(int status_code, std::string_view status_name)
  {
    code = status_code;
    name = status_name;
  }

  /// Implicit conversion from Status_t& -> Status
  ///
  /// @param status - the status to copy
  constexpr Status(const Status_t & status)  // NOLINT
  {
    code = status.code;
    name = status.name;
  }

  /// Implicit conversion from Status_t&& -> Status
  ///
  /// @param status - the status to copy
  constexpr Status(const Status_t && status)  // NOLINT
  {
    code = status.code;
    name = status.name;
  }

  /// Implicit assignment from Status_t& -> Status
  ///
  /// @param status
  /// @return constexpr Status
  constexpr Status operator=(const Status_t & status)
  {
    code = status.code;
    name = status.name;
    return *this;
  }

  /// Implicit assignment from Status_t&& -> Status
  ///
  /// @param status
  /// @return constexpr Status
  constexpr Status operator=(const Status_t && status)
  {
    code = status.code;
    name = status.name;
    return *this;
  }

  /// A status indicating that the operation was successful.
  /// This is deprecated in ErrorHandling v2 as there is no longer a need for
  /// a non-error like status code.
  static constexpr auto kSuccess = CreateStatus(0, "Success");

  /// Used to indicate that a particular operation could not be accomplished
  /// in the time given.
  static constexpr auto kTimedOut = CreateStatus(-1, "Timed Out");

  /// A problem in communicate over a communication channel occurred,
  /// resulting in an operation failing.
  static constexpr auto kBusError = CreateStatus(-2, "Bus Error");

  /// Failure occurred because an operation required a device to be present,
  /// and it was not.
  static constexpr auto kDeviceNotFound = CreateStatus(-3, "Device Not Found");

  /// A system or object failed to perform an operation due to the fact that
  /// the settings are not correct. An example of this could be setting the
  /// clock speed of a system to 0 Hz, and then attempting to use that system
  /// for communication or generating a signal.
  static constexpr auto kInvalidSettings = CreateStatus(-4, "Invalid Settings");

  /// This is returned when the implementation of a particular aspect of an
  /// interface was purposefully kept unimplemented. This can happen in cases
  /// where a particular implementation, does not have the capability to
  /// perform
  static constexpr auto kNotImplemented = CreateStatus(-5, "Not Implemented");

  /// This is returned if a system is currently busy and cannot take new
  /// operations or requests.
  static constexpr auto kNotReadyYet = CreateStatus(-6, "Not Ready Yet");

  /// Failure to perform an operation because the input parameters were not
  /// acceptable for the function or method being called.
  static constexpr auto kInvalidParameters =
      CreateStatus(-7, "Invalid Parameters");

  /// Unfinished is returned when a particular aspect of an implementation of
  /// a system, class or object is unfinished, but is planned to be finished.
  /// This is different from kNotImplemented, where there is no plan to ever
  /// implement a particular feature.
  static constexpr auto kUnfinished = CreateStatus(-8, "Unfinished");
};

/// Operator == definitions between Status_t and Status objects.
///
/// @param rhs Status to compare
/// @param lhs Status to compare
/// @return true - if the error codes are the same.
constexpr bool operator==(const Status & rhs, const Status & lhs)
{
  return rhs.code == lhs.code;
}

/// Operator != definitions between Status_t and Status objects.
///
/// @param rhs Status to compare
/// @param lhs Status to compare
/// @return true - if the error codes are the same.
constexpr bool operator!=(const Status & rhs, const Status & lhs)
{
  return rhs.code != lhs.code;
}

/// Backwards compability for IsOK(enum) for when Status was a enum type
///
/// @param status - The status to check. If the code is 0, then it is ok.
/// @return true - if the status is a successful one.
constexpr bool IsOk(const Status & status)
{
  return status.code == Status::kSuccess.code;
}

/// Backwards compability for Value(enum) for when Status. was a enum type
///
/// @param status -
/// @return constexpr int - return the status.code
constexpr int Value(const Status & status)
{
  return status.code;
}

/// Backwards compatibility for Stringify(enum) for when Status. was a enum
/// type
///
/// @param status
/// @return constexpr const char*
constexpr const char * Stringify(const Status_t & status)
{
  return status.name.data();
}

/// Error object that contains the Status code, message and location of where an
/// error occurred. This is the underlying error type of SJSU-Dev2.
struct Error_t
{
  /// Represents an empty string
  constexpr static std::string_view kEmptyMessage = "";

  /// @param error_status   - status code to go with this error object
  /// @param error_message  - message to go with error to describe exactly why
  ///                         the error occurred.
  /// @param source_location - the location in the source code where the Error_t
  ///                          was created.
  constexpr Error_t(Status error_status,
                    std::string_view error_message = kEmptyMessage,
                    const std::experimental::source_location & source_location =
                        std::experimental::source_location::current())
      : status(error_status), message(kEmptyMessage), location(source_location)
  {
    if constexpr (config::kStoreErrorMessages)
    {
      message = error_message;
    }
    if constexpr (config::kAutomaticallyPrintOnError)
    {
      if (!std::is_constant_evaluated())
      {
        Print();
        if constexpr (config::kIncludeBacktrace)
        {
          printf(SJ2_HI_BOLD_WHITE "Backtrace:" SJ2_COLOR_RESET);
          int depth = 0;
          _Unwind_Backtrace(&debug::PrintAddressInRow, &depth);
          puts("");
        }
      }
    }
  }

  /// Print the error message to STDOUT
  void Print()
  {
    /// Print the colored error text to STDOUT
    printf(SJ2_BOLD_YELLOW "Error:" SJ2_HI_BOLD_RED "%s(%d)" SJ2_HI_BOLD_WHITE
                           ":%s:%" PRIuLEAST32 ":%s(): %s\n" SJ2_COLOR_RESET,
           status.name.data(), status.code, location.file_name(),
           location.line(), location.function_name(), message.data());
  }

  constexpr bool operator==(const Error_t & other) const
  {
    return status == other.status;
  }

  /// The status associated with this error
  Status status;
  /// Custom message describing the error
  std::string_view message = kEmptyMessage;
  /// Location of where the error occurred.
  std::experimental::source_location location;
};

/// Compile time factory function for cleanly creating
/// std::unexpected<Error_t> objects. Library code shall use this function to
/// generate std::unexpected objects and shall not use std::unexpected
/// directly to keep the code style consistent.
///
/// Usage:
///
///     return Error(Status::kTimeout, "Couldn't find resource in time");
///
/// Without this function:
///
///     return std::unexpected(
///               Error_t{Status::kTimeout, "Couldn't find resource in
///               time"});
///
/// @param status - The status associated with this error
/// @param message - The custom message to go with the status
/// @param location - Default initialized and should almost never be supplied
///                   by the user. Will be defaulted to the location in which
///                   this function was called.
/// @return constexpr tl::unexpected<Error_t>
constexpr tl::unexpected<Error_t> Error(
    Status status,
    std::string_view message = Error_t::kEmptyMessage,
    const std::experimental::source_location & location =
        std::experimental::source_location::current())
{
  return tl::unexpected(Error_t{ status, message, location });
}

/// Alias for the more complex tl::expected definitions. Using
/// std::expected<T, Error_t> should never be used in order to conform to the
/// SJSU-Dev2 standard.
///
/// @tparam T - the actual, non-error, result to return from the function
template <typename T>
using Returns = tl::expected<T, Error_t>;

template <typename T>
constexpr Returns<T> NoError()
{
  return T{};
}

/// This is a helper function for the `SJ2_RETURN_ON_ERROR()` macro to help it
/// return the results of a `Returns<>` object.
///
/// std::expected has a special case where the `value()` does not exist for
/// void types, meaning that in that specific case, we cannot call that
/// method. So in the place of running `value()` we return a throw away value,
/// in this case int 0.
///
/// @tparam T - deduced type of the `Returns<T>` object. This value should not
///             be supplied.
/// @param result - the result of an expression that returns a `Returns<T>`
///                 object.
/// @return constexpr auto - returns 0 if the type is void, otherwise this
///         will return the value held within the result object.
template <typename T>
constexpr auto GetReturnValue(Returns<T> & result)
{
  if constexpr (std::is_void_v<T>)
  {
    return 0;
  }
  else
  {
    return result.value();
  }
}

/// A macro for simplifying the boiler plate of evaluating an expression that
/// returns a Returns<> object. This will handle evaluating the function that
/// returns a Returns<> object and if it is an error, it will return/bubble up
/// the error code rather than continuing through the code. Otherwise, this
/// macro will return the value of the result.
///
/// Usage examples can be shown at the top of the file.
#define SJ2_RETURN_ON_ERROR(expression)       \
  ({                                          \
    auto _result = (expression);              \
    if (!_result)                             \
    {                                         \
      return tl::unexpected(_result.error()); \
    }                                         \
    GetReturnValue(_result);                  \
  })

/// Does the same as SJ2_RETURN_ON_ERROR, except this can be used to return any
/// sort of value, in the value parameter.
#define SJ2_RETURN_VALUE_ON_ERROR(expression, value) \
  ({                                                 \
    auto _result = (expression);                     \
    if (!_result)                                    \
    {                                                \
      return value;                                  \
    }                                                \
    GetReturnValue(_result);                         \
  })

}  // namespace sjsu

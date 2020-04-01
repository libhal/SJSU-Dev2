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
struct Status;

struct Status_t
{
  /// The status code number
  int code = -1;
  /// The string representation of the status.
  std::string_view name = "Unknown";
  constexpr Status_t(int status_code, std::string_view status_name)
      : code(status_code), name(status_name)
  {
  }

  bool operator!() const
  {
    return code != 0;
  }
};

constexpr Status_t CreateStatus(int status_code, std::string_view status_name)
{
  return Status_t{ status_code, status_name };
}

/// Status error codes returned by functions throughout the SJSU-Dev2 code
/// base.
struct Status
{
  /// The status code number
  int code = -1;
  /// The string representation of the status.
  std::string_view name = "Unknown";

  constexpr Status() {}
  /// Factory object that is used to simplify the creation of status objects.
  ///
  /// @param code - The status code
  /// @param status_name - string to represent the name of the status.
  constexpr Status(int status_code, std::string_view status_name)
      : code(status_code), name(status_name)
  {
  }

  /// Factory object that is used to simplify the creation of status objects.
  ///
  /// @param code - The status code
  /// @param status_name - string to represent the name of the status.
  constexpr Status(const Status_t & status)  // NOLINT
      : code(status.code), name(status.name)
  {
  }
  constexpr Status(const Status_t && status)  // NOLINT
      : code(status.code), name(status.name)
  {
  }

  constexpr Status operator=(const Status_t & status)
  {
    code = status.code;
    name = status.name;
    return *this;
  }
  constexpr Status operator=(const Status_t && status)
  {
    code = status.code;
    name = status.name;
    return *this;
  }

  bool operator!() const
  {
    return code != 0;
  }

  explicit operator int() const
  {
    return static_cast<int>(code);
  }
  explicit operator uint8_t() const
  {
    return static_cast<uint8_t>(code);
  }
  explicit operator uint16_t() const
  {
    return static_cast<uint16_t>(code);
  }
  explicit operator uint32_t() const
  {
    return static_cast<uint32_t>(code);
  }

  /// [[deprecated]] A status indicating that the operation was successful.
  /// This is deprecated in ErrorHandling v2 as there is no longer a need for a
  /// non-error like status code.
  static constexpr auto kSuccess = CreateStatus(0, "Success");

  /// Used to indicate that a particular operation could not be accomplished in
  /// the time given.
  static constexpr auto kTimedOut = CreateStatus(1, "Timed Out");

  /// A problem in communicate over a communication channel occurred, resulting
  /// in an operation failing.
  static constexpr auto kBusError = CreateStatus(2, "Bus Error");

  /// Failure occurred because an operation required a device to be present, and
  /// it was not.
  static constexpr auto kDeviceNotFound = CreateStatus(3, "Device Not Found");

  /// A system or object failed to perform an operation due to the fact that the
  /// settings are not correct. An example of this could be setting the clock
  /// speed of a system to 0 Hz, and then attempting to use that system for
  /// communication or generating a signal.
  static constexpr auto kInvalidSettings = CreateStatus(4, "Invalid Settings");

  /// This is returned when the implementation of a particular aspect of an
  /// interface was purposefully kept unimplemented. This can happen in cases
  /// where a particular implementation, does not have the capability to perform
  static constexpr auto kNotImplemented = CreateStatus(5, "Not Implemented");

  /// This is returned if a system is currently busy and cannot take new
  /// operations or requests.
  static constexpr auto kNotReadyYet = CreateStatus(6, "Not Ready Yet");

  /// Failure to perform an operation because the input parameters were not
  /// acceptable for the function or method being called.
  static constexpr auto kInvalidParameters =
      CreateStatus(7, "Invalid Parameters");

  /// Unfinished is returned when a particular aspect of an implementation of a
  /// system, class or object is unfinished, but is planned to be finished. This
  /// is different from kNotImplemented, where there is no plan to ever
  /// implement a particular feature.
  static constexpr auto kUnfinished = CreateStatus(8, "Unfinished");
};

constexpr bool operator==(const Status & rhs, const Status & lhs)
{
  return rhs.code == lhs.code;
}

constexpr bool operator!=(const Status & rhs, const Status & lhs)
{
  return rhs.code != lhs.code;
}

constexpr bool operator==(const Status_t & rhs, const Status & lhs)
{
  return rhs.code == lhs.code;
}

constexpr bool operator!=(const Status_t & rhs, const Status & lhs)
{
  return rhs.code != lhs.code;
}

// TODO(): Remove this once full migration has occurred.
/// @return true - if the status is a successful one.
constexpr bool IsOk(const Status & status)
{
  return status.code == Status::kSuccess.code;
}

constexpr int Value(const Status & status)
{
  return status.code;
}

constexpr const char * Stringify(const Status & status)
{
  return status.name.data();
}

struct Error_t
{
  /// Represents an empty string
  constexpr static std::string_view kEmptyMessage = "";

  /// Constructs the Error_t object
  ///
  /// @param status   - status code to go with this error object
  /// @param message  - message to go with error to describe exactly why the
  ///                   error occurred.
  /// @param location - the location in the source code where the Error_t was
  ///                   created.
  constexpr Error_t(Status error_status,
                    std::string_view error_message = kEmptyMessage,
                    const std::experimental::source_location & source_location =
                        std::experimental::source_location::current())
      : status(error_status), message(error_message), location(source_location)
  {
    // TODO(): BRING THESE BACK
    // if constexpr (config::kStoreMessage)
    // {
    //   message_ = message;
    // }
    // if constexpr (config::kAutoExceptionPrint)
    // {
    //   print();
    // }
  }

  void Print()
  {
    printf(SJ2_BOLD_YELLOW "Error:" SJ2_HI_BOLD_RED "%s(%d)" SJ2_HI_BOLD_WHITE
                           ":%s:%" PRIuLEAST32 ":%s(): " SJ2_COLOR_RESET,
           status.name.data(), status.code, location.file_name(),
           location.line(), location.function_name());
    if (kEmptyMessage != message)
    {
      printf("%s", message.data());
    }
    printf(SJ2_HI_BOLD_WHITE "\nBacktrace:" SJ2_COLOR_RESET);
    int depth = 0;
    _Unwind_Backtrace(&debug::PrintAddressInRow, &depth);
    puts("");
  }

  Status status;
  std::string_view message = kEmptyMessage;
  std::experimental::source_location location;
};

constexpr tl::unexpected<Error_t> Error(
    Status status,
    std::string_view message = Error_t::kEmptyMessage,
    const std::experimental::source_location & location =
        std::experimental::source_location::current())
{
  return tl::unexpected(Error_t{ status, message, location });
}

template <typename T>
using Returns = tl::expected<T, Error_t>;

///
///
/// @tparam T
/// @param a
/// @return T
template <typename T>
constexpr T GetReturnValue(Returns<T> & a)
{
  return a.value();
}

///
///
/// @tparam T
/// @param a
/// @return const T
template <typename T>
constexpr const T GetReturnValue(const Returns<T> & a)
{
  return a.value();
}

/// In the special case where the return value is void, we return an int 0
/// to allow the SJ2_RETURN_ON_ERROR() macro to continue to work.
constexpr int GetReturnValue(Returns<void> &)
{
  return 0;
}

#define SJ2_RETURN_ON_ERROR(expression)       \
  ({                                          \
    auto _result = (expression);              \
    if (!_result)                             \
    {                                         \
      return tl::unexpected(_result.error()); \
    }                                         \
    GetValue(_result);                        \
  })

}  // namespace sjsu

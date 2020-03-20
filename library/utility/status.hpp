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

#define SJ2_RETURN_ON_ERROR(expression)       \
  ({                                          \
    auto _result = (expression);              \
    if (!_result)                             \
    {                                         \
      return tl::unexpected(_result.error()); \
    }                                         \
    _result.value();                          \
  })

#define SJ2_RETURN_ON_ERROR_VOID(expression)  \
  {                                           \
    auto _result = (expression);              \
    if (!_result)                             \
    {                                         \
      return tl::unexpected(_result.error()); \
    }                                         \
  }

namespace sjsu
{
/// Status_t error codes returned by functions throughout the SJSU-Dev2 code
/// base.
struct Status_t
{
  /// The status code number
  int code = std::numeric_limits<int>::max();
  /// The string representation of the status.
  std::string_view name = "Unknown";

  bool operator!() const
  {
    return code != 0;
  }

  bool operator==(const Status_t & status) const
  {
    return status.code == code;
  }

  bool operator!=(const Status_t & status) const
  {
    return status.code == code;
  }
};

/// Factory object that is used to simplify the creation of status objects.
///
/// @tparam kCode - The status code
template <size_t kCode>
class CreateStatus : public Status_t
{
 public:
  /// Constructs the Status_t parent with the status_name as its name and the
  /// kCode template parameter as its status code.
  ///
  /// @param status_name - string to represent the name of the status.
  constexpr CreateStatus(std::string_view status_name)
  {
    code = kCode;
    name = status_name;
  }

  bool operator!() const
  {
    return code != 0;
  }

  bool operator==(const Status_t & status) const
  {
    return status.code == code;
  }

  bool operator!=(const Status_t & status) const
  {
    return status.code == code;
  }
};

namespace Status
{
/// [[deprecated]] A status indicating that the operation was successful.
/// This is deprecated in ErrorHandling v2 as there is no longer a need for a
/// non-error like status code.
constexpr auto kSuccess = CreateStatus<0>("Success");

/// Used to indicate that a particular operation could not be accomplished in
/// the time given.
constexpr auto kTimedOut = CreateStatus<1>("Timed Out");

/// A problem in communicate over a communication channel occurred, resulting in
/// an operation failing.
constexpr auto kBusError = CreateStatus<2>("Bus Error");

/// Failure occurred because an operation required a device to be present, and
/// it was not.
constexpr auto kDeviceNotFound = CreateStatus<3>("Device Not Found");

/// A system or object failed to perform an operation due to the fact that the
/// settings are not correct. An example of this could be setting the clock
/// speed of a system to 0 Hz, and then attempting to use that system for
/// communication or generating a signal.
constexpr auto kInvalidSettings = CreateStatus<4>("Invalid Settings");

/// This is returned when the implementation of a particular aspect of an
/// interface was purposefully kept unimplemented. This can happen in cases
/// where a particular implementation, does not have the capability to perform
constexpr auto kNotImplemented = CreateStatus<5>("Not Implemented");

/// This is returned if a system is currently busy and cannot take new
/// operations or requests.
constexpr auto kNotReadyYet = CreateStatus<6>("Not Ready Yet");

/// Failure to perform an operation because the input parameters were not
/// acceptable for the function or method being called.
constexpr auto kInvalidParameters = CreateStatus<7>("Invalid Parameters");

/// Unfinished is returned when a particular aspect of an implementation of a
/// system, class or object is unfinished, but is planned to be finished. This
/// is different from kNotImplemented, where there is no plan to ever implement
/// a particular feature.
constexpr auto kUnfinished = CreateStatus<8>("Unfinished");
};  // namespace Status

// TODO(): Remove this once full migration has occurred.
/// @return true - if the status is a successful one.
constexpr bool IsOk(const Status_t & status)
{
  return status.code == Status::kSuccess.code;
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
  constexpr Error_t(Status_t status,
                    std::string_view message = kEmptyMessage,
                    const std::experimental::source_location & location =
                        std::experimental::source_location::current())
      : status_(status), message_(message), location_(location)
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
    print();
  }

  void print()
  {
    printf(SJ2_BOLD_YELLOW "Error:" SJ2_HI_BOLD_RED "%s(%d)" SJ2_HI_BOLD_WHITE
                           ":%s:%" PRIuLEAST32 ":%s(): " SJ2_COLOR_RESET,
           status_.name.data(), status_.code, location_.file_name(),
           location_.line(), location_.function_name());
    if (kEmptyMessage != message_)
    {
      printf("%s", message_.data());
    }
    printf(SJ2_HI_BOLD_WHITE "\nBacktrace:" SJ2_COLOR_RESET);
    int depth = 0;
    _Unwind_Backtrace(&debug::PrintAddressInRow, &depth);
    puts("");
  }

  Status_t status_;
  std::string_view message_ = kEmptyMessage;
  const std::experimental::source_location location_;
};

constexpr tl::unexpected<Error_t> Error(
    Status_t status,
    std::string_view message = Error_t::kEmptyMessage,
    const std::experimental::source_location & location =
        std::experimental::source_location::current())
{
  return tl::unexpected(Error_t{ status, message, location });
}

template <typename T>
using Returns = tl::expected<T, Error_t>;

}  // namespace sjsu

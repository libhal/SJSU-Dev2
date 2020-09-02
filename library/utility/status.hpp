/// For information on how to use this library please see:
///
///     demos/multiplatform/status
///
#pragma once

#include <cinttypes>
#include <limits>
#include <string_view>
#include <system_error>

#if defined(__clang_analyzer__)
#include "utility/dummy/source_location"
#else
#include <experimental/source_location>
#endif

#include "utility/debug.hpp"
#include "utility/enum.hpp"
#include "third_party/expected/include/tl/expected.hpp"

namespace sjsu
{
constexpr const char * Stringify(std::errc error_code)
{
  if constexpr (config::kStoreErrorCodeStrings)
  {
    switch (error_code)
    {
      case std::errc::address_family_not_supported:
        return "address family not supported";

      case std::errc::address_in_use: return "address in use";
      case std::errc::address_not_available: return "address not available";
      case std::errc::already_connected: return "already connected";
      case std::errc::argument_list_too_long: return "argument list too long";
      case std::errc::argument_out_of_domain: return "argument out of domain";
      case std::errc::bad_address: return "bad address";
      case std::errc::bad_file_descriptor: return "bad file descriptor";
      case std::errc::bad_message: return "bad message";
      case std::errc::broken_pipe: return "broken pipe";
      case std::errc::connection_aborted: return "connection aborted";
      case std::errc::connection_already_in_progress:
        return "connection already in progress";

      case std::errc::connection_refused: return "connection refused";
      case std::errc::connection_reset: return "connection reset";
      case std::errc::cross_device_link: return "cross device link";
      case std::errc::destination_address_required:
        return "destination address required";

      case std::errc::device_or_resource_busy: return "device or resource busy";
      case std::errc::directory_not_empty: return "directory not empty";
      case std::errc::executable_format_error: return "executable format error";
      case std::errc::file_exists: return "file exists";
      case std::errc::file_too_large: return "file too large";
      case std::errc::filename_too_long: return "filename too long";
      case std::errc::function_not_supported: return "function not supported";
      case std::errc::host_unreachable: return "host unreachable";
      case std::errc::identifier_removed: return "identifier removed";
      case std::errc::illegal_byte_sequence: return "illegal byte sequence";
      case std::errc::inappropriate_io_control_operation:
        return "inappropriate io control operation";

      case std::errc::interrupted: return "interrupted";
      case std::errc::invalid_argument: return "invalid argument";
      case std::errc::invalid_seek: return "invalid seek";
      case std::errc::io_error: return "io error";
      case std::errc::is_a_directory: return "is a directory";
      case std::errc::message_size: return "message size";
      case std::errc::network_down: return "network down";
      case std::errc::network_reset: return "network reset";
      case std::errc::network_unreachable: return "network unreachable";
      case std::errc::no_buffer_space: return "no buffer space";
      case std::errc::no_child_process: return "no child process";
      case std::errc::no_link: return "no link";
      case std::errc::no_lock_available: return "no lock available";
      case std::errc::no_message_available: return "no message available";
      case std::errc::no_message: return "no message";
      case std::errc::no_protocol_option: return "no protocol option";
      case std::errc::no_space_on_device: return "no space on device";
      case std::errc::no_stream_resources: return "no stream resources";
      case std::errc::no_such_device_or_address:
        return "no such device or address";

      case std::errc::no_such_device: return "no such device";
      case std::errc::no_such_file_or_directory:
        return "no such file or directory";

      case std::errc::no_such_process: return "no such process";
      case std::errc::not_a_directory: return "not a directory";
      case std::errc::not_a_socket: return "not a socket";
      case std::errc::not_a_stream: return "not a stream";
      case std::errc::not_connected: return "not connected";
      case std::errc::not_enough_memory: return "not enough memory";
      case std::errc::not_supported: return "not supported";
      case std::errc::operation_canceled: return "operation canceled";
      case std::errc::operation_in_progress: return "operation in progress";
      case std::errc::operation_not_permitted: return "operation not permitted";
      case std::errc::owner_dead: return "owner dead";
      case std::errc::permission_denied: return "permission denied";
      case std::errc::protocol_error: return "protocol error";
      case std::errc::protocol_not_supported: return "protocol not supported";
      case std::errc::read_only_file_system: return "read only file system";
      case std::errc::resource_deadlock_would_occur:
        return "resource deadlock would occur";

      case std::errc::resource_unavailable_try_again:
        return "resource unavailable try again";

      case std::errc::result_out_of_range: return "result out of range";
      case std::errc::state_not_recoverable: return "state not recoverable";
      case std::errc::stream_timeout: return "stream timeout";
      case std::errc::text_file_busy: return "text file busy";
      case std::errc::timed_out: return "timed out";
      case std::errc::too_many_files_open_in_system:
        return "too many files open in system";

      case std::errc::too_many_files_open: return "too many files open";
      case std::errc::too_many_links: return "too many links";
      case std::errc::too_many_symbolic_link_levels:
        return "too many symbolic link levels";

      case std::errc::value_too_large: return "value too large";
      case std::errc::wrong_protocol_type: return "wrong protocol type";
      default:
      {
        return "unknown";
      }
    }
  }
  return "unknown";
}

/// Error object that contains the Status code, message and location of where an
/// error occurred. This is the underlying error type of SJSU-Dev2.
class Error_t  // NOLINT
{
 public:
  /// Represents an empty string
  constexpr static const char * kEmptyMessage = "";

  /// @param error_code    - error code to be associated with this error object
  /// @param error_message - message to go with error to describe exactly why
  ///                         the error occurred.
  /// @param source_location - the location in the source code where the Error_t
  ///                          was created.
  constexpr Error_t(std::errc error_code,
                    const char * error_message = kEmptyMessage,
                    const std::experimental::source_location & source_location =
                        std::experimental::source_location::current())
      : code(error_code)
  {
    file     = source_location.file_name();
    function = source_location.function_name();
    line     = source_location.line();

    if constexpr (config::kStoreErrorMessages)
    {
      message = error_message;
    }
  }

  /// Print the error message to STDOUT
  void Print() const
  {
    /// Print the colored error text to STDOUT
    printf(SJ2_BOLD_YELLOW "Error:" SJ2_HI_BOLD_RED "%s(%d)" SJ2_HI_BOLD_WHITE
                           ":%s:%d:%s(): %s\n" SJ2_COLOR_RESET,
           Stringify(code), Value(code), file, line, function, message);
  }

  constexpr bool operator==(const Error_t & other) const
  {
    return code == other.code;
  }

  /// @return true if `code` contains an error_code which is any non-zero
  /// number.
  constexpr operator bool() const
  {
    return static_cast<int>(code) != 0;
  }

  friend constexpr bool operator==(const Error_t & lhs, std::errc rhs)
  {
    return rhs == lhs.code;
  }

  friend constexpr bool operator==(std::errc lhs, const Error_t & rhs)
  {
    return lhs == rhs.code;
  }

  /// The status associated with this error
  std::errc code;
  // /// The category associated with the error type
  // const std::error_category & category;
  /// Custom message describing the error
  const char * message = kEmptyMessage;
  /// File name string
  const char * file = kEmptyMessage;
  /// Function name string
  const char * function = kEmptyMessage;
  /// File line number
  int line = 0;
};

/// Short hand for writing `tl::unexpected(error);` when returning a bare
/// Error_t type.
///
/// @param error - the error type to return.
constexpr tl::unexpected<const Error_t *> DefinedError(const Error_t & error)
{
  return tl::unexpected(&error);
}

/// Alias for the more complex tl::expected definitions. Using
/// tl::expected<T, Error_t> should never be used in order to conform to the
/// SJSU-Dev2 standard.
///
/// @tparam T - the actual, non-error, result to return from the function
template <typename T>
using Returns = tl::expected<T, const Error_t *>;

template <typename T>
constexpr Returns<T> NoError()
{
  return T{};
}

/// Checks if the Returns<T> object contains an error and if so, does it match
/// the error_code provided. Will automatically be false if the Return<T> does
/// not contain an error.
///
/// Shorthand to allow comparison between a Returns<T> object and an error code
/// enumeration.
///
/// This allows you to migrate code from
///
///    std::errc::invalid_argument == result.error()->code;
///
/// To:
///
///    std::errc::invalid_argument == result
///
/// @param error_code - enumeration to compare with the Returns<T> object.
/// @param result - the result to compare against the error code.
template <typename T>
constexpr bool operator==(std::errc error_code, const Returns<T> & result)
{
  if (result.has_value())
  {
    return false;
  }
  return error_code == result.error()->code;
}

/// Checks if the Returns<T> object contains an error and if so, does it match
/// the error_code provided. Will automatically be false if the Return<T> does
/// not contain an error.
///
/// Shorthand to allow comparison between a Returns<T> object and an error code
/// enumeration.
///
/// This allows you to migrate code from
///
///    std::errc::invalid_argument == result.error()->code;
///
/// To:
///
///    std::errc::invalid_argument == result
///
/// @param error_code - enumeration to compare with the Returns<T> object.
/// @param result - the result to compare against the error code.
template <typename T>
constexpr bool operator==(const Returns<T> & result, std::errc error_code)
{
  if (result.has_value())
  {
    return false;
  }
  return error_code == result.error()->code;
}

/// This is a helper function for the `SJ2_RETURN_ON_ERROR()` macro to help it
/// return the results of a `Returns<>` object.
///
/// tl::expected has a special case where the `value()` does not exist for
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

/// Compile time factory function for cleanly creating
/// tl::unexpected<Error_t> objects. Library code shall use this function to
/// generate tl::unexpected objects and shall not use tl::unexpected
/// directly to keep the code style consistent.
///
/// Usage:
///
///     return Error(std::errc::timed_out, "Couldn't find resource in time");
///
/// Without this function:
///
///     return tl::unexpected(Error_t{std::errc::timed_out,
///                                   "Couldn't find resource in time"});
///
/// @param error - The error_condition associated with this error
/// @param message - The custom message to go with the status
/// @return constexpr tl::unexpected<Error_t>
#define Error(error, message)                                       \
  ({                                                                \
    constexpr static ::sjsu::Error_t _static_error(error, message); \
    auto unexpected_result = tl::unexpected(&_static_error);        \
    unexpected_result;                                              \
  })

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

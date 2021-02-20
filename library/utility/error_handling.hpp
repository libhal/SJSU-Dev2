/// For information on how to use this library please see:
///
///     demos/multiplatform/status
///
#pragma once

#include <system_error>

#if defined(__clang_analyzer__)
#include "utility/dummy/source_location"
#else
#include <experimental/source_location>
#endif

#include "config.hpp"
#include "third_party/expected/include/tl/expected.hpp"

namespace sjsu
{
/// @return constexpr const char* - the string representation of the std::errc
/// code passed.
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

/// The standard exception class for SJSU-Dev2
class Exception : std::exception
{
 public:
  /// Default message for exceptions without a context message.
  constexpr static const char * kEmptyMessage = "(undefined)";

  /// @param error_code_enum - error code that represents this error
  /// @param location_message - a message that descripes the exception and why
  /// it occurred.
  /// @param source_location - location of where this exception was created.
  explicit Exception(
      std::errc error_code_enum,
      const char * location_message = kEmptyMessage,
      const std::experimental::source_location & source_location =
          std::experimental::source_location::current())
      : code_(error_code_enum)
  {
    message_  = location_message;
    file_     = source_location.file_name();
    function_ = source_location.function_name();
    line_     = source_location.line();
  }

  /// @return const char* - return the message associated with this excpetion
  const char * what() const noexcept override
  {
    return message_;
  }

  /// Print the error message to STDOUT
  void Print() const
  {
    /// Print the colored error text to STDOUT
    printf("Error:%s(%d):%s:%d:%s(): %s\n",
           Stringify(code_),
           static_cast<int>(code_),
           file_,
           line_,
           function_,
           message_);
  }

  /// @return std::errc - the error code number
  std::errc GetCode() const
  {
    return code_;
  }

  /// Check if the exception object has and error code equal to the compared
  /// error code
  ///
  /// @param exception - the exception object to compare
  /// @param error - the error code to check against
  /// @return true - `exception` has the same error code as `error`
  /// @return false - `exception` does NOT have the same error code as `error`
  friend bool operator==(const Exception & exception, std::errc error)
  {
    return exception.GetCode() == error;
  }

  /// Check if the exception object has and error code equal to the compared
  /// error code
  ///
  /// @param error - the error code to check against
  /// @param exception - the exception object to compare
  /// @return true - `exception` has the same error code as `error`
  /// @return false - `exception` does NOT have the same error code as `error`
  friend bool operator==(std::errc error, const Exception & exception)
  {
    return exception.GetCode() == error;
  }

 private:
  // Error code associated with this exception
  std::errc code_ = std::errc{};

  // Helpful exception handling for debugging and logging purposes
  const char * message_ = kEmptyMessage;

  /// File name string
  const char * file_ = kEmptyMessage;

  /// Function name string
  const char * function_ = kEmptyMessage;

  /// File line number
  int line_ = 0;
};
}  // namespace sjsu

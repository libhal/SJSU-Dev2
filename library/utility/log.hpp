/// @ingroup SJSU-Dev2
/// @defgroup Log Utility Functions
/// @brief This module is meant for general purpose macros that can be used
/// across the SJSU-Dev2 environment.
/// @{
#pragma once

/// This is required for Apple/OSX as source_location is not available on
/// Apple's CLANG 10 which is the default clang headers on OSX. Not to be
/// confused by LLVM's actual CLANG 10.
#if defined(__clang_analyzer__)
#include "utility/dummy/source_location"
#else
#include <experimental/source_location>
#endif

#include <algorithm>
#include <array>
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <string_view>
#include <type_traits>

#include "config.hpp"
#include "log_levels.hpp"

#include "utility/ansi_terminal_codes.hpp"
#include "utility/constexpr.hpp"
#include "utility/debug.hpp"
#include "utility/macros.hpp"
#include "utility/time.hpp"
#include "utility/status.hpp"

namespace sjsu
{
/// Variadic Log object that labels the log with a preceeding "DEBUG" label.
/// Will only log if the SJ2_LOG_LEVEL is level SJ2_LOG_LEVEL_DEBUG or greater.
///
/// @tparam Params - Variadic type array to describe the params variable pack.
template <typename... Params>
struct Log  // NOLINT
{
  /// On construction this object will print a log statement with the "DEBUG"
  /// label preceeding it. See example below:
  /// Example:
  ///
  ///     INFO:main.cpp:main():13> Hello World 0x4
  ///
  /// @param log_type - the log prefix like "INFO", "DEBUG", "ERROR", etc...
  /// @param format - format string to be used for logging
  /// @param params - variadic list of parameters to be passed to the log object
  /// @param location - the location in the source code where this object was
  ///        constructed.
  Log(const char * log_type,
      const char * format,
      Params... params,
      const std::experimental::source_location & location =
          std::experimental::source_location::current())
  {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"

    // Container for the formatted log string
    std::array<char, config::kPrintfBufferSize> buffer;

    // Write log prefix statement to the buffer.
    int position =
        snprintf(buffer.data(), buffer.size(),
                 "%s" SJ2_HI_BLUE ":%s:" SJ2_HI_GREEN "%s():" SJ2_HI_YELLOW
                 "%" PRIuLEAST32 "> " SJ2_WHITE,
                 log_type, FileBasename(location.file_name()),
                 location.function_name(), location.line());

    int bytes_left = 0;

    // Write the actual log message into the buffer
    bytes_left = std::max(static_cast<int>(buffer.size()) - position, 0);
    position += snprintf(&buffer[position], bytes_left, format, params...);

    // Write the ending of the log statement that appends a newline and a color
    // reset command.
    bytes_left = std::max(static_cast<int>(buffer.size()) - position, 0);
    position += snprintf(&buffer[position], bytes_left, "\n" SJ2_COLOR_RESET);

    // If we have exceeded the limits of the log statement, we truncate the
    // message and add an "..." at the end, to indicate that data was
    // trimmed/lost.
    if (static_cast<size_t>(position) >= buffer.size() - 1)
    {
      static constexpr std::string_view kEllipsisEnding =
          "...\n" SJ2_COLOR_RESET;
      position = buffer.size() - (kEllipsisEnding.size() + 1);
      std::copy(kEllipsisEnding.begin(), kEllipsisEnding.end(),
                &buffer[position]);
    }

    // Finally use fputs to pass buffer to stdout, typically UART port or
    // semihost.
    fputs(buffer.data(), stdout);

#pragma GCC diagnostic pop
  }
};

/// Specialized log object that labels the log with a preceeding "DEBUG" label.
/// Will only log if the SJ2_LOG_LEVEL is level SJ2_LOG_LEVEL_DEBUG or greater.
///
/// @tparam Params - Variadic type array to describe the params variable pack.
template <typename... Params>
struct LogDebug  // NOLINT
{
  /// On construction this object will print a log statement with the "DEBUG"
  /// label preceeding it.
  ///
  /// @param format - format string to be used for logging
  /// @param params - variadic list of parameters to be passed to the log object
  /// @param location - the location in the source code where this object was
  ///        constructed.
  LogDebug(const char * format,
           Params... params,
           const std::experimental::source_location & location =
               std::experimental::source_location::current())
  {
    // Required as GCC8 has parsing issues with the C++17 [[maybe_unused]]
    // attribute for constructor's first argument
    _SJ2_USED(format);
    if constexpr (config::kLogLevel >= SJ2_LOG_LEVEL_DEBUG)
    {
      Log<Params...>(SJ2_BACKGROUND_PURPLE "   DEBUG", format, params...,
                     location);
    }
  }
};

/// Specialized log object that labels the log with a preceeding "INFO" label.
/// Will only log if the SJ2_LOG_LEVEL is level SJ2_LOG_LEVEL_INFO or greater.
///
/// @tparam Params - Variadic type array to describe the params variable pack.
template <typename... Params>
struct LogInfo  // NOLINT
{
  /// On construction this object will print a log statement with the "INFO"
  /// label preceeding it.
  ///
  /// @param format - format string to be used for logging
  /// @param params - variadic list of parameters to be passed to the log object
  /// @param location - the location in the source code where this object was
  ///        constructed.
  LogInfo(const char * format,
          Params... params,
          const std::experimental::source_location & location =
              std::experimental::source_location::current())
  {
    // Required as GCC8 has parsing issues with the C++17 [[maybe_unused]]
    // attribute for constructor's first argument
    _SJ2_USED(format);
    if constexpr (config::kLogLevel >= SJ2_LOG_LEVEL_INFO)
    {
      Log<Params...>(SJ2_BACKGROUND_GREEN "    INFO", format, params...,
                     location);
    }
  }
};

/// Specialized log object that labels the log with a preceeding "WARNING"
/// label.
/// Will only log if the SJ2_LOG_LEVEL is level SJ2_LOG_LEVEL_WARNING or
/// greater.
///
/// @tparam Params - Variadic type array to describe the params variable pack.
template <typename... Params>
struct LogWarning  // NOLINT
{
  /// On construction this object will print a log statement with the "WARNING"
  /// label preceeding it.
  ///
  /// @param format - format string to be used for logging
  /// @param params - variadic list of parameters to be passed to the log object
  /// @param location - the location in the source code where this object was
  ///        constructed.
  LogWarning(const char * format,
             Params... params,
             const std::experimental::source_location & location =
                 std::experimental::source_location::current())
  {
    // Required as GCC8 has parsing issues with the C++17 [[maybe_unused]]
    // attribute for constructor's first argument
    _SJ2_USED(format);
    if constexpr (config::kLogLevel >= SJ2_LOG_LEVEL_WARNING)
    {
      Log<Params...>(SJ2_BACKGROUND_YELLOW " WARNING", format, params...,
                     location);
    }
  }
};

/// Specialized log object that labels the log with a preceeding "ERROR" label.
/// Will only log if the SJ2_LOG_LEVEL is level SJ2_LOG_LEVEL_ERROR or greater.
///
/// @tparam Params - Variadic type array to describe the params variable pack.
template <typename... Params>
struct LogError  // NOLINT
{
  /// On construction this object will print a log statement with the "ERROR"
  /// label preceeding it.
  ///
  /// @param format - format string to be used for logging
  /// @param params - variadic list of parameters to be passed to the log object
  /// @param location - the location in the source code where this object was
  ///        constructed.
  LogError(const char * format,
           Params... params,
           const std::experimental::source_location & location =
               std::experimental::source_location::current())
  {
    // Required as GCC8 has parsing issues with the C++17 [[maybe_unused]]
    // attribute for constructor's first argument
    _SJ2_USED(format);
    if constexpr (config::kLogLevel >= SJ2_LOG_LEVEL_ERROR)
    {
      Log<Params...>(SJ2_BACKGROUND_RED "   ERROR", format, params...,
                     location);
    }
  }
};

/// Deduction guide for Log
template <typename... Params>
Log(const char * log_type, const char * format, Params...)->Log<Params...>;

/// Deduction guide for LogDebug
template <typename... Params>
LogDebug(const char * format, Params...)->LogDebug<Params...>;

/// Deduction guide for LogInfo
template <typename... Params>
LogInfo(const char * format, Params...)->LogInfo<Params...>;

/// Deduction guide for LogWarning
template <typename... Params>
LogWarning(const char * format, Params...)->LogWarning<Params...>;

/// Deduction guide for LogError
template <typename... Params>
LogError(const char * format, Params...)->LogError<Params...>;
}  // namespace sjsu

/// Deprecated log macro with the DEBUG level of log message.
#define LOG_DEBUG(format, ...) ::sjsu::LogDebug(format, ##__VA_ARGS__)

/// Deprecated log macro with the INFO level of log message.
#define LOG_INFO(format, ...) ::sjsu::LogInfo(format, ##__VA_ARGS__)

/// Deprecated log macro with the WARNING level of log message.
#define LOG_WARNING(format, ...) ::sjsu::LogWarning(format, ##__VA_ARGS__)

/// Deprecated log macro with the ERROR level of log message.
#define LOG_ERROR(format, ...) ::sjsu::LogError(format, ##__VA_ARGS__)

/// When the condition is false, issue a warning to the user with a warning
/// message. Warning message format acts like printf.
#define SJ2_ASSERT_WARNING(condition, warning_message, ...) \
  do                                                        \
  {                                                         \
    if (!(condition))                                       \
    {                                                       \
      ::sjsu::LogWarning(warning_message, ##__VA_ARGS__);   \
    }                                                       \
  } while (0)

/// Returns and prinparams statement if condition returns true
#define SJ2_RETURN_IF(condition, warning_message, ...)                    \
  do                                                                      \
  {                                                                       \
    if ((condition))                                                      \
    {                                                                     \
      ::sjsu::LogWarning(warning_message SJ2_COLOR_RESET, ##__VA_ARGS__); \
    }                                                                     \
  } while (0)

/// Logs the expression if it returns any but Status::kSuccess
#define LOG_ON_FAILURE(expression)                              \
  do                                                            \
  {                                                             \
    sjsu::Status log_on_failure_status = (expression);          \
    if (log_on_failure_status != sjsu::Status::kSuccess)        \
    {                                                           \
      ::sjsu::LogWarning("Expression Failed: %s", #expression); \
    }                                                           \
  } while (0)

/// When the condition is false, issue a critical level message to the user and
/// halt the processor.
#define SJ2_ASSERT_FATAL_WITH_DUMP(with_dump, condition, fatal_message, ...) \
  do                                                                         \
  {                                                                          \
    if (!(condition))                                                        \
    {                                                                        \
      ::sjsu::LogError("Assertion Failure, Condition Tested: " #condition    \
                       "\n          " fatal_message SJ2_COLOR_RESET,         \
                       ##__VA_ARGS__);                                       \
      if ((with_dump) && ::config::kIncludeBacktrace)                        \
      {                                                                      \
        printf("\nPrinting Stack Trace:\n\n");                               \
        ::sjsu::debug::PrintBacktrace(true);                                 \
      }                                                                      \
      ::sjsu::Halt();                                                        \
    }                                                                        \
  } while (0)

/// If the condition if found ot be false, print the expression and dump the
/// backtrace.
#if defined(HOST_TEST)
#define SJ2_ASSERT_FATAL(condition, fatal_message, ...)                \
  /* Without the if statement using the (condition) and _SJ2_USED() */ \
  /* the compiler may complain about unused variables.              */ \
  /* This serves to silence those warnings during host tests.       */ \
  if (condition)                                                       \
  {                                                                    \
    _SJ2_USED(fatal_message);                                          \
  }
#else
#define SJ2_ASSERT_FATAL(condition, fatal_message, ...) \
  SJ2_ASSERT_FATAL_WITH_DUMP(true, (condition), fatal_message, ##__VA_ARGS__)
#endif  // defined(HOST_TEST)
/// Print a variable using the printf_specifier supplied.
#define SJ2_PRINT_VARIABLE(variable, printf_specifier) \
  ::sjsu::LogError(#variable " = " printf_specifier, (variable))
/// @}

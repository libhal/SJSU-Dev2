/// @ingroup SJSU-Dev2
/// @defgroup Log Utility Functions
/// @brief This module is meant for general purpose macros that can be used
/// across the SJSU-Dev2 environment.
/// @{
#pragma once

#if defined(__clang_analyzer__)
#include "utility/dummy/source_location"
#else
#include <experimental/source_location>
#endif

#include <cinttypes>
#include <cstdio>
#include <cstring>

#include "config.hpp"
#include "log_levels.hpp"

#include "utility/ansi_terminal_codes.hpp"
#include "utility/constexpr.hpp"
#include "utility/debug.hpp"
#include "utility/macros.hpp"
#include "utility/time.hpp"

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
    printf("%s" SJ2_HI_BLUE ":%s:" SJ2_HI_GREEN "%s():" SJ2_HI_YELLOW
           "%" PRIuLEAST32 "> " SJ2_WHITE,
           log_type,
           FileBasename(location.file_name()),
           location.function_name(),
           location.line());

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-security"
    // Without the pragmas disabling the format security warning, this will give
    // a warning everytime it is used.
    printf(format, params...);
#pragma GCC diagnostic pop

    puts(SJ2_COLOR_RESET);
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
    if constexpr (config::kLogLevel <= SJ2_LOG_LEVEL_DEBUG)
    {
      Log<Params...>(
          SJ2_BACKGROUND_PURPLE "   DEBUG", format, params..., location);
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
    if constexpr (config::kLogLevel <= SJ2_LOG_LEVEL_INFO)
    {
      Log<Params...>(
          SJ2_BACKGROUND_GREEN "    INFO", format, params..., location);
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
    if constexpr (config::kLogLevel <= SJ2_LOG_LEVEL_WARNING)
    {
      Log<Params...>(
          SJ2_BACKGROUND_YELLOW " WARNING", format, params..., location);
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
    if constexpr (config::kLogLevel <= SJ2_LOG_LEVEL_ERROR)
    {
      Log<Params...>(
          SJ2_BACKGROUND_RED "   ERROR", format, params..., location);
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

/// Log with the DEBUG level of log mesage.
#define LOG_DEBUG(format, ...) ::sjsu::LogDebug(format, ##__VA_ARGS__)

/// Log with the INFO level of log mesage.
#define LOG_INFO(format, ...) ::sjsu::LogInfo(format, ##__VA_ARGS__)

/// Log with the WARNING level of log mesage.
#define LOG_WARNING(format, ...) ::sjsu::LogWarning(format, ##__VA_ARGS__)

/// Log with the ERROR level of log mesage.
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
      if ((with_dump))                                                       \
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

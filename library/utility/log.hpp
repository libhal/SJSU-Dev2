/// @ingroup SJSU-Dev2
/// @defgroup Macros Utility Macros
/// @brief This module is meant for general purpose macros that can be used
/// across the SJSU-Dev2 environment.
/// @{
#pragma once
#include <cstdio>

#include "config.hpp"
#include "log_levels.hpp"

#include "utility/ansi_terminal_codes.hpp"
#include "utility/constexpr.hpp"
#include "utility/debug.hpp"
#include "utility/macros.hpp"
#include "utility/time.hpp"

/// SJ2_LOG_FUNCTION is an alias of __PRETTY_FUNCTION__ in the event we would
/// like to switch the function to something else.
#if SJ2_DESCRIPTIVE_FUNCTION_NAME
#define SJ2_LOG_FUNCTION __PRETTY_FUNCTION__
#else
#define SJ2_LOG_FUNCTION ""
#endif
/// Printf style logging with filename, function name, and line number
#define _LOG_PRINT(log_message, format, ...)                                  \
  do                                                                          \
  {                                                                           \
    static constexpr ::sjsu::FileBasename_t<::sjsu::StringLength(__FILE__),   \
                                            ::sjsu::BasenameLength(__FILE__)> \
        file(__FILE__);                                                       \
    printf(log_message SJ2_HI_BLUE ":%s:" SJ2_HI_GREEN "%s:" SJ2_HI_YELLOW    \
                                   "%d> " SJ2_WHITE format SJ2_COLOR_RESET    \
                                   "\n",                                      \
           file.basename,                                                     \
           SJ2_LOG_FUNCTION,                                                  \
           __LINE__,                                                          \
           ##__VA_ARGS__);                                                    \
  } while (0)
/// Log with the DEBUG level of log mesage.
#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_DEBUG
#define LOG_DEBUG(format, ...) _LOG_PRINT("   DEBUG", format, ##__VA_ARGS__)
#else
#define LOG_DEBUG(format, ...)
#endif  // SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_DEBUG

/// Log with the INFO level of log mesage.
#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_INFO
#define LOG_INFO(format, ...) \
  _LOG_PRINT(SJ2_BACKGROUND_GREEN "    INFO", format, ##__VA_ARGS__)
#else
#define LOG_INFO(format, ...)
#endif  // SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_INFO

/// Log with the WARNING level of log mesage.
#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_WARNING
#define LOG_WARNING(format, ...) \
  _LOG_PRINT(SJ2_BACKGROUND_YELLOW " WARNING", format, ##__VA_ARGS__)
#else
#define LOG_WARNING(format, ...)
#endif  // SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_WARNING

/// Log with the ERROR level of log mesage.
#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_ERROR
#define LOG_ERROR(format, ...) \
  _LOG_PRINT(SJ2_BACKGROUND_PURPLE "   ERROR", format, ##__VA_ARGS__)
#else
#define LOG_ERROR(format, ...)
#endif  // SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_ERROR

/// When the condition is false, issue a warning to the user with a warning
/// message. Warning message format acts like printf.
#define SJ2_ASSERT_WARNING(condition, warning_message, ...)        \
  do                                                               \
  {                                                                \
    if (!(condition))                                              \
    {                                                              \
      LOG_WARNING(warning_message SJ2_COLOR_RESET, ##__VA_ARGS__); \
    }                                                              \
  } while (0)

/// When the condition is false, issue a critical level message to the user and
/// halt the processor.
#define SJ2_ASSERT_FATAL_WITH_DUMP(with_dump, condition, fatal_message, ...) \
  do                                                                         \
  {                                                                          \
    if (!(condition))                                                        \
    {                                                                        \
      LOG_ERROR("Assertion Failure, Condition Tested: " #condition           \
                "\n          " fatal_message SJ2_COLOR_RESET,                \
                ##__VA_ARGS__);                                              \
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
#if defined HOST_TEST
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
#endif  // defined HOST_TEST
/// Print a variable using the printf_specifier supplied.
#define SJ2_PRINT_VARIABLE(variable, printf_specifier) \
  LOG_INFO(#variable " = " printf_specifier, (variable))
/// @}

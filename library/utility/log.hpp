/// @ingroup SJSU-Dev2
/// @defgroup Macros Utility Macros
/// @brief This module is meant for general purpose macros that can be used
/// across the SJSU-Dev2 environment.
/// @{
#pragma once

#include <cinttypes>
#include <cstdarg>
#include <cstdio>
#include <experimental/source_location>

#include "config.hpp"
#include "log_levels.hpp"

#include "utility/ansi_terminal_codes.hpp"
#include "utility/debug.hpp"
#include "utility/macros.hpp"
#include "utility/time.hpp"

namespace sjsu
{
struct Log
{
  constexpr Log(std::experimental::source_location const location =
                    std::experimental::source_location::current())
      : location_{ location }
  {
  }
  void log_location(const char * log_level, const char * color = "")
  {
    printf("%s%8s" SJ2_HI_BLUE ":%s" SJ2_HI_GREEN ":%s()" SJ2_HI_YELLOW
           ":%" PRIuLEAST32 "> " SJ2_COLOR_RESET,
           color,
           log_level,
           location_.file_name(),
           location_.function_name(),
           location_.line());
  }

  void debug(const char * format, ...)
  {
    log_location("DEBUG");

    va_list va;
    va_start(va, format);
    vprintf(format, va);
    va_end(va);
    puts("");
  }

  void info(const char * format, ...)
  {
    log_location("INFO", SJ2_BACKGROUND_GREEN);

    va_list va;
    va_start(va, format);
    vprintf(format, va);
    va_end(va);
    puts("");
  }

  void warning(const char * format, ...)
  {
    log_location("WARNING", SJ2_BACKGROUND_PURPLE);

    va_list va;
    va_start(va, format);
    vprintf(format, va);
    va_end(va);
    puts("");
  }

  void error(const char * format, ...)
  {
    log_location("ERROR", SJ2_BACKGROUND_RED);

    va_list va;
    va_start(va, format);
    vprintf(format, va);
    va_end(va);
    puts("");
  }

 private:
  std::experimental::source_location const location_;
};
}  // namespace sjsu

#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_DEBUG
#define LOG_DEBUG(format, ...) ::sjsu::Log().debug(format, ##__VA_ARGS__)
#else
#define LOG_DEBUG(format, ...)
#endif  // SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_DEBUG

/// Log with the INFO level of log mesage.
#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_INFO
#define LOG_INFO(format, ...) ::sjsu::Log().info(format, ##__VA_ARGS__)
#else
#define LOG_INFO(format, ...)
#endif  // SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_INFO

/// Log with the WARNING level of log mesage.
#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_WARNING
#define LOG_WARNING(format, ...) ::sjsu::Log().warning(format, ##__VA_ARGS__)
#else
#define LOG_WARNING(format, ...)
#endif  // SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_WARNING

/// Log with the ERROR level of log mesage.
#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_ERROR
#define LOG_ERROR(format, ...) ::sjsu::Log().error(format, ##__VA_ARGS__)
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

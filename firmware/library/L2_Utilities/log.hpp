#pragma once
#include <cstdio>

#include "config.hpp"
#include "log_levels.hpp"

#include "L2_Utilities/ansi_terminal_codes.hpp"
#include "L2_Utilities/constexpr.hpp"
#include "L2_Utilities/debug.hpp"
#include "L2_Utilities/macros.hpp"
#include "L2_Utilities/time.hpp"

#if SJ2_DESCRIPTIVE_FUNCTION_NAME
#define SJ2_LOG_FUNCTION __PRETTY_FUNCTION__
#else
#define SJ2_LOG_FUNCTION ""
#endif
// Printf style logging with filename, function name, and line number
#if SJ2_DEBUG_PRINT_ENABLED
#define DEBUG_PRINT(format, ...)                                      \
  do                                                                  \
  {                                                                   \
    static constexpr FileBasename_t<StringLength(__FILE__),           \
                                    BasenameLength(__FILE__)>         \
        file(__FILE__);                                               \
    printf(SJ2_HI_BLUE "%s:" SJ2_HI_GREEN "%s:" SJ2_HI_YELLOW         \
                       "%d> " SJ2_WHITE format SJ2_COLOR_RESET "\n",  \
           file.basename, SJ2_LOG_FUNCTION, __LINE__, ##__VA_ARGS__); \
  } while (0)
#else
#define DEBUG_PRINT(format, ...)
#endif  // SJ2_DEBUG_PRINT_ENABLED

// Printf style logging with filename, function name, and line number
#define LOG_PRINT(log_message, format, ...)                                \
  do                                                                       \
  {                                                                        \
    static constexpr FileBasename_t<StringLength(__FILE__),                \
                                    BasenameLength(__FILE__)>              \
        file(__FILE__);                                                    \
    printf(log_message SJ2_HI_BLUE ":%s:" SJ2_HI_GREEN "%s:" SJ2_HI_YELLOW \
                                   "%d> " SJ2_WHITE format SJ2_COLOR_RESET \
                                   "\n",                                   \
           file.basename, SJ2_LOG_FUNCTION, __LINE__, ##__VA_ARGS__);      \
  } while (0)

#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_DEBUG
#define LOG_DEBUG(format, ...) LOG_PRINT("   DEBUG", format, ##__VA_ARGS__)
#else
#define LOG_DEBUG(format, ...)
#endif  // SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_DEBUG

#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_INFO
#define LOG_INFO(format, ...) \
  LOG_PRINT(SJ2_BACKGROUND_GREEN "    INFO", format, ##__VA_ARGS__)
#else
#define LOG_INFO(format, ...)
#endif  // SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_INFO

#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_WARNING
#define LOG_WARNING(format, ...) \
  LOG_PRINT(SJ2_BACKGROUND_YELLOW " WARNING", format, ##__VA_ARGS__)
#else
#define LOG_WARNING(format, ...)
#endif  // SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_WARNING

#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_ERROR
#define LOG_ERROR(format, ...) \
  LOG_PRINT(SJ2_BACKGROUND_PURPLE "   ERROR", format, ##__VA_ARGS__)
#else
#define LOG_ERROR(format, ...)
#endif  // SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_ERROR

#if SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_CRITICAL
#define LOG_CRITICAL(format, ...) \
  LOG_PRINT(SJ2_BACKGROUND_RED "CRITICAL", format, ##__VA_ARGS__)
#else
#define LOG_CRITICAL(format, ...)
#endif  // SJ2_LOG_LEVEL <= SJ2_LOG_LEVEL_CRITICAL

// When the condition is false, issue a warning to the user with a warning
// message. Warning message format acts like printf.
#define SJ2_ASSERT_WARNING(condition, warning_message, ...)        \
  do                                                               \
  {                                                                \
    if (!(condition))                                              \
    {                                                              \
      LOG_WARNING(warning_message SJ2_COLOR_RESET, ##__VA_ARGS__); \
    }                                                              \
  } while (0)
// When the condition is false, issue a critical level message to the user and
// halt the processor.
#define SJ2_ASSERT_FATAL_WITH_DUMP(with_dump, condition, fatal_message, ...)  \
  do                                                                          \
  {                                                                           \
    if (!(condition))                                                         \
    {                                                                         \
      LOG_CRITICAL("Assertion Failure, Condition Tested: " #condition         \
                   "\n          " fatal_message SJ2_COLOR_RESET,             \
                   ##__VA_ARGS__);                                            \
      if ((with_dump))                                                        \
      {                                                                       \
        printf("\nPrinting Stack Trace:\n\n");                                \
        ::debug::PrintBacktrace();                                            \
        printf(                                                               \
            "\nRun: the following command in your project directory"          \
            "\n\n    " SJ2_BOLD_WHITE                                         \
            "arm-none-eabi-addr2line -e build/binaries/firmware.elf "         \
            "<insert pc>" SJ2_COLOR_RESET                                     \
            "\n\n"                                                            \
            "This will report the file and line number associated with that " \
            "program counter values provided above in the backtrace.\n\n");   \
      }                                                                       \
      Halt();                                                                 \
    }                                                                         \
  } while (0)

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

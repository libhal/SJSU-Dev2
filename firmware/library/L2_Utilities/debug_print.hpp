#pragma once
#include <cstdio>

#include "config.hpp"
#include "log_levels.hpp"

#include "L2_Utilities/ansi_terminal_codes.hpp"
#include "L2_Utilities/constexpr.hpp"
#include "L2_Utilities/macros.hpp"

// Constructing this structure with a file path, will generate a structure with
// a character array containing just the bare basename.
template <size_t kPathLength, size_t kBasenameLength>
struct FileBasename_t
{
  constexpr explicit FileBasename_t(const char (&path)[kPathLength])
      : basename{}
  {
    size_t base_position = 0;
    for (size_t i = kPathLength - kBasenameLength; i < kPathLength; i++)
    {
      basename[base_position] = path[i];
      base_position++;
    }
    basename[base_position - 1] = '\0';
  }
  char basename[kBasenameLength];
};

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

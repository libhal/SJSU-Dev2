#pragma once
#include <cstdio>
#include "config.hpp"
#include "L2_Utilities/ansi_terminal_codes.hpp"
#include "L2_Utilities/constexpr.hpp"

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

// Printf style logging with filename, function name, and line number
#if SJ2_DEBUG_PRINT_ENABLED
#define DEBUG_PRINT(format, ...)                                         \
  do                                                                     \
  {                                                                      \
    static constexpr FileBasename_t<StringLength(__FILE__),              \
                                    BasenameLength(__FILE__)>            \
        file(__FILE__);                                                  \
    printf(SJ2_HI_BLUE "%s:" SJ2_HI_GREEN "%s:" SJ2_HI_YELLOW            \
                       "%d> " SJ2_WHITE format SJ2_COLOR_RESET "\n",     \
           file.basename, __PRETTY_FUNCTION__, __LINE__, ##__VA_ARGS__); \
  } while (0)
#else
#define DEBUG_PRINT(format, ...)
#endif  // SJ2_DEBUG_PRINT_ENABLED

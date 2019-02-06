#pragma once

#include <cctype>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "config.hpp"
#include "newlib/newlib.hpp"
#include "utility/ansi_terminal_codes.hpp"
#include "utility/macros.hpp"

namespace debug
{
// Hexdump Utility Functions
static inline void PrintCharacterRow(uint8_t * bytes, size_t length)
{
  putchar('|');
  for (size_t j = 0; j < length; j++)
  {
    if (isprint(bytes[j]))
    {
      putchar(bytes[j]);
    }
    else
    {
      putchar('.');
    }
  }
  puts("|");
}

static inline void PrintHexBytesRow(uint8_t * bytes, size_t length)
{
  for (size_t j = 0; j < 16; j++)
  {
    if (j < length)
    {
      printf("%02X ", bytes[j]);
    }
    else
    {
      printf("   ");
    }
    if (j == 7)
    {
      putchar(' ');
    }
  }
  putchar(' ');
}

inline void Hexdump(void * address, uint32_t length)
{
  uint8_t * bytes = static_cast<uint8_t *>(address);
  for (uint32_t i = 0; i < length; i += 16)
  {
    printf("%08" PRIX32 "  ", i);
    size_t bytes_to_print = (i + 15 > length) ? (length % 16) : 16;
    PrintHexBytesRow(&bytes[i], bytes_to_print);
    PrintCharacterRow(&bytes[i], bytes_to_print);
  }
  printf("%08" PRIX32 "  \n", length);
}

[[gnu::no_instrument_function]]
inline void PrintBacktrace(bool show_make_command = false,
                           void * final_address   = nullptr)
{
  printf("Stack Depth = %zd\n", GetStackDepth());
  // stack_depth-1 to ignore PrintBacktrace()
  // PrintBacktrace shouldn't be ignored in profiling because it causes
  // the exit to still fire, which can result in a negative stack_depth
  void ** list_of_called_functions = GetStackTrace();
  size_t stack_depth               = GetStackDepth();
  // Ignore the last function as it is the Backtrace function
  for (size_t pos = 0; pos < stack_depth - 1; pos++)
  {
    printf("  #%zu: 0x%p\n", pos, list_of_called_functions[pos]);
  }
  if (final_address != nullptr)
  {
    printf("  #%zu: 0x%p\n", stack_depth - 1, final_address);
  }
  if (show_make_command)
  {
    constexpr const char kBuildType[] =
#if defined(APPLICATION)
        "application";
#else
        "bootloader";
#endif
    puts("\nRun: the following command in your project directory");
    printf("\n  " SJ2_BOLD_WHITE "make stacktrace-%s TRACES=\"", kBuildType);
    for (size_t pos = 0; pos < stack_depth - 1; pos++)
    {
      if (pos != 0)
      {
        putchar(' ');
      }
      printf("0x%p", list_of_called_functions[pos]);
    }
    if (final_address != nullptr)
    {
      printf(" 0x%p", final_address);
    }
    puts("\"\n" SJ2_COLOR_RESET);
    puts(
        "This will report the file and line number that led to this function "
        "being called.");
  }
}

}  // namespace debug

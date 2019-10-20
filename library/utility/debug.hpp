#pragma once

#include <unwind.h>

#include <cctype>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "config.hpp"
#include "newlib/newlib.hpp"
#include "utility/ansi_terminal_codes.hpp"
#include "utility/build_info.hpp"
#include "utility/macros.hpp"

namespace sjsu
{
namespace debug
{
// =====================================
// Hidden utility functions for Hexdump
// =====================================
inline void PrintCharacterRow(uint8_t * bytes, size_t length)
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

inline void PrintHexBytesRow(uint8_t * bytes, size_t length)
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

/// Similar to the UNIX hexdump program, this function will read the bytes from
/// the starting address and print them in hex. Any characters that can be
/// translated into a viewable character will be display.
///
/// Example Usage:
///
///   char some_string[] = "Hello World, and Goodbye World!\n";
///   // Subtract 1 to ignore null character
///   sjsu::debug::Hexdump(some_string, sizeof(some_string) - 1);
///
/// Output:
/*
  00000000  48 65 6c 6c 6f 20 57 6f  72 6c 64 2c 20 61 6e 64  |Hello World, and|
  00000010  20 47 6f 6f 64 62 79 65  20 57 6f 72 6c 64 21 0a  | Goodbye World!.|
  00000020
*/
///
/// @param address - location to start reading bytes from
/// @param length - the number of bytes to read from the starting location
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

// ==============================================
// Hidden Backtrace Utility Functions
// ==============================================
inline _Unwind_Reason_Code PrintAddressAsList(_Unwind_Context * context,
                                                     void * depth_pointer)
{
  int * depth      = static_cast<int *>(depth_pointer);
  intptr_t address = static_cast<intptr_t>(_Unwind_GetIP(context));
  printf("  %d) 0x%08" PRIXPTR "\n", *depth,
         address - config::kBacktraceAddressOffset);
  (*depth)++;
  return _URC_NO_REASON;
}
inline _Unwind_Reason_Code PrintAddressInRow(_Unwind_Context * context,
                                                    void * depth_pointer)
{
  int * depth      = static_cast<int *>(depth_pointer);
  intptr_t address = static_cast<intptr_t>(_Unwind_GetIP(context));
  printf(" 0x%08" PRIXPTR, address - config::kBacktraceAddressOffset);
  (*depth)++;
  return _URC_NO_REASON;
}
/// Similar to the UNIX hexdump program, this function will read the bytes from
/// the starting address and print them in hex. Any characters that can be
/// translated into a viewable character will be display.
///
/// Example Usage:
///
///   sjsu::debug::PrintBacktrace();
///
/// @param show_make_command - if true, print the make command that can be used
///        to print the file and line number that corresponds to the printed
///        addresses.
/// @param length - the number of bytes to read from the starting location
inline void PrintBacktrace(bool show_make_command = false,
                           void * final_address   = nullptr)
{
  if constexpr (config::kIncludeBacktrace)
  {
    int depth = 0;
    _Unwind_Backtrace(&PrintAddressAsList, &depth);
    if (final_address)
    {
      printf("  %d) 0x%p\n", depth, final_address);
    }

    if (show_make_command)
    {
      printf("\nRun: the following command in your project directory");
      printf("\n\n  " SJ2_BOLD_WHITE);
      printf("make stacktrace-%s TRACES=\"", Stringify(build::kTarget));

      _Unwind_Backtrace(&PrintAddressInRow, &depth);
      if (final_address)
      {
        printf(" 0x%p", final_address);
      }

      printf("\"\n\n" SJ2_COLOR_RESET);
      printf(
          "This will report the file and line number that led to this function "
          "being called.\n");
    }
  }
}
}  // namespace debug
}  // namespace sjsu

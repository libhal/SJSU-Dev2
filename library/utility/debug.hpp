#pragma once

#include <unwind.h>

#include <algorithm>
#include <array>
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
inline void PrintCharacterRow(const uint8_t * bytes,
                              size_t length,
                              char * buffer,
                              int position)
{
  buffer[position++] = '|';
  for (size_t j = 0; j < length; j++)
  {
    if (isprint(bytes[j]))
    {
      buffer[position++] = bytes[j];
    }
    else
    {
      buffer[position++] = '.';
    }
  }
  buffer[position++] = '|';
  buffer[position++] = '\n';
}

inline int PrintHexBytesRow(const uint8_t * bytes,
                            size_t length,
                            char * buffer,
                            int position)
{
  for (size_t j = 0; j < 16; j++)
  {
    if (j < length)
    {
      position += snprintf(&buffer[position], 4, "%02X ", bytes[j]);  // NOLINT
    }
    else
    {
      position += snprintf(&buffer[position], 4, "   ");  // NOLINT
    }

    if (j == 7)
    {
      buffer[position++] = ' ';
    }
  }
  buffer[position++] = ' ';
  return position;
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
/// @param address - location to start reading bytes from
/// @param length - the number of bytes to read from the starting location
template <size_t kNumberOfRowsBuffered = 1>
inline void Hexdump(void * address, size_t length)
{
  // Verify that the number of rows specified is 1 or more
  static_assert(kNumberOfRowsBuffered > 0,
                "Number of buffered rows must be greater than zero.");

  // The number of bytes required to hold a row of hexdump data + newline
  static constexpr size_t kRowMaximumLength = 79;

  // Define a buffer that will contain the bytes for each row of the hex dump.
  // +1 to size for NULL CHARACTER.
  std::array<char, (kNumberOfRowsBuffered * kRowMaximumLength) + 1> rows;

  // Fill the rows
  std::fill(rows.begin(), rows.end(), '\0');

  // Convert the void* to a uint8_t* so that the value are interpreted as
  // numerical bytes.
  uint8_t * bytes = static_cast<uint8_t *>(address);

  size_t row_position = 0;

  for (size_t i = 0; i < length; i += 16, row_position++)
  {
    // If the row position goes above the number rows, reset the rows and print
    // them.
    if (row_position >= kNumberOfRowsBuffered)
    {
      printf("%s", rows.data());
      std::fill(rows.begin(), rows.end(), '\0');
      row_position = 0;
    }

    // Point to the start of the current rows.
    char * row = &rows[row_position * kRowMaximumLength];

    // Print the starting address position of the row
    int position = snprintf(row, 11, "%08zX  ", i);  // NOLINT

    // Figure out the number of bytes to display
    size_t bytes_to_print = (i + 15 > length) ? (length % 16) : 16;

    position = PrintHexBytesRow(&bytes[i], bytes_to_print, row, position);
    PrintCharacterRow(&bytes[i], bytes_to_print, row, position);
  }

  // Print out the last of the data rows
  printf("%s", rows.data());

  // Print out the amount of data
  printf("%08zX  \n", length);
}

/// Generate a string of text that represents a hexdump of any data type.
///
/// @tparam Structure - the inferred type of the address.
/// @param address - address of the structure to be convereted into a hexdumped
/// array.
/// @return auto - std::array<char, N> where N is the number characters needed
/// to represent the hexdump data.
template <class Structure>
inline auto HexdumpStructure(const Structure & address)
{
  static constexpr size_t kLength = sizeof(Structure);
  // The number of bytes required to hold a row of hexdump data + newline
  static constexpr size_t kRowMaximumLength = 79;

  // Define a buffer that will contain the bytes for each row of the hex dump.
  // +1 to size for NULL CHARACTER.
  std::array<char, ((kLength + 1) * kRowMaximumLength)> rows;

  // Fill the rows
  std::fill(rows.begin(), rows.end(), '\0');

  // Convert the void* to a uint8_t* so that the value are interpreted as
  // numerical bytes.
  const uint8_t * bytes = reinterpret_cast<const uint8_t *>(&address);

  size_t row_position = 0;

  for (size_t i = 0; i < kLength; i += 16, row_position++)
  {
    // Point to the start of the current rows.
    char * row = &rows[row_position * kRowMaximumLength];

    // Print the starting address position of the row
    int position = snprintf(row, 11, "%08zX  ", i);  // NOLINT

    // Figure out the number of bytes to display
    size_t bytes_to_print = (i + 15 > kLength) ? (kLength % 16) : 16;

    position = PrintHexBytesRow(&bytes[i], bytes_to_print, row, position);
    PrintCharacterRow(&bytes[i], bytes_to_print, row, position);
  }

  return rows;
}

/// Only prints hexdump if the log level for the application is above DEBUG
template <size_t kNumberOfRowsBuffered = 1>
inline void HexdumpDebug(void * address, size_t length)
{
  if constexpr (config::kLogLevel <= SJ2_LOG_LEVEL_DEBUG)
  {
    Hexdump<kNumberOfRowsBuffered>(address, length);
  }
}

// ==============================================
// Hidden Backtrace Utility Functions
// ==============================================
inline _Unwind_Reason_Code PrintAddressAsList(_Unwind_Context * context,
                                              void * depth_pointer)
{
  int * depth      = static_cast<int *>(depth_pointer);
  intptr_t address = static_cast<intptr_t>(_Unwind_GetIP(context));
  printf("  %d) 0x%08" PRIXPTR "\n",
         *depth,
         address - config::kBacktraceAddressOffset);
  (*depth)++;
  return _URC_NO_REASON;
}

inline _Unwind_Reason_Code PrintAddressInRow(_Unwind_Context * context,
                                             void * depth_pointer)
{
  int * depth      = static_cast<int *>(depth_pointer);
  intptr_t address = static_cast<intptr_t>(_Unwind_GetIP(context));
  printf("0x%08" PRIXPTR " ", address - config::kBacktraceAddressOffset);
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
/// @param final_address - when this address is spotted, terminate the backtrace
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
      printf("make stacktrace PLATFORM=%s TRACES=\"",
             build::Stringify(build::kPlatform));

      _Unwind_Backtrace(&PrintAddressInRow, &depth);
      if (final_address)
      {
        printf("0x%p", final_address);
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

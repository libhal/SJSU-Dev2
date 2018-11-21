#pragma once

#include <cctype>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "config.hpp"
#include "L2_Utilities/macros.hpp"
#include "newlib/newlib.hpp"

namespace debug
{
// Hexdump Utility Functions
static inline void PrintCharacterRow(uint8_t * bytes, size_t length)
{
  printf("|");
  for (size_t j = 0; j < length; j++)
  {
    if (isprint(bytes[j]))
    {
      printf("%c", bytes[j]);
    }
    else
    {
      printf(".");
    }
  }
  printf("|\n");
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
      printf(" ");
    }
  }
  printf(" ");
}

inline void Hexdump(void * address, uint32_t length)
{
  uint8_t * bytes = static_cast<uint8_t *>(address);
  uint32_t i      = 0;
  for (i = 0; i < length; i += 16)
  {
    printf("%08" PRIX32 "  ", i);
    size_t bytes_to_print = (i + 15 > length) ? (i + 15) - length : 16;
    PrintHexBytesRow(&bytes[i], bytes_to_print);
    PrintCharacterRow(&bytes[i], bytes_to_print);
  }
  printf("%08" PRIX32 "  \n", length);
}

inline void PrintBacktrace()
{
  printf("Stack Depth = %zd\n", GetStackDepth());
  // stack_depth-1 to ignore PrintBacktrace()
  // PrintBacktrace shouldn't be ignored in profiling because it causes
  // the exit to still fire, which can result in a negative stack_depth
  void ** list_of_called_functions = GetStackTrace();
  size_t stack_depth = GetStackDepth();
  for (size_t pos = 0; pos < stack_depth; pos++)
  {
    printf("  #%zu: 0x%p\n", pos, list_of_called_functions[pos]);
  }
}

}  // namespace debug

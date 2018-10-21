#include "L2_Utilities/debug.hpp"

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
namespace
{
// =============================
// Hexdump Utility Functions
// =============================
void PrintCharacterRow(uint8_t * bytes, size_t length)
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

void PrintHexBytesRow(uint8_t * bytes, size_t length)
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
// =============================
// Backtrace Utility Functions
// =============================
// Not ignoring the profile functions within the stack trace will result in
// an recursive loop.
extern "C"
SJ2_IGNORE_STACK_TRACE(void __cyg_profile_func_enter(void*, void*)); // NOLINT

extern "C"
SJ2_IGNORE_STACK_TRACE(void __cyg_profile_func_exit(void*, void*)); // NOLINT

void * stack_trace[config::kBacktraceDepth];
size_t stack_depth = 0;

extern "C" void __cyg_profile_func_enter(void *, void * call_site)  // NOLINT
{
  stack_trace[stack_depth++] = call_site;
}

extern "C" void __cyg_profile_func_exit(void *, void *)  // NOLINT
{
  stack_depth--;
}
}  // namespace

void Hexdump(void * address, uint32_t length)
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

void PrintTrace()
{
  printf("Stack Depth = %zd\n", stack_depth);
  // stack_depth-1 to ignore PrintTrace()
  // PrintTrace shouldn't be ignored in profiling because it causes the exit to
  // still fire, which results in a negative stack_depth
  for (size_t pos = 0; pos < stack_depth; pos++)
  {
    printf("  #%zu: 0x%p\n", pos, stack_trace[pos]);
  }
}

}  // namespace debug

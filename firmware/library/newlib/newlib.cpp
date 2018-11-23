#include "newlib/newlib.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>

#include "L1_Drivers/uart.hpp"
#include "L2_Utilities/macros.hpp"
#include "L4_Application/globals.hpp"

int PutChar(int data)
{
  uart0.Send(static_cast<uint8_t>(data));
  return 1;
}

int GetChar()
{
  return static_cast<int>(uart0.Receive());
}

#if defined(HOST_TEST)
Stdout out = putchar;
Stdin in   = getchar;
#else
Stdout out = PutChar;
Stdin in   = GetChar;
#endif

extern "C"
{
  // Dummy implementation of isatty
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _isatty([[maybe_unused]] int file)
  {
    return 1;
  }
  // Dummy implementation of exit with return code placed into
  // Arm register r3
  // NOLINTNEXTLINE(readability-identifier-naming)
  void _exit([[maybe_unused]] int rc)
  {
    while (1)
    {
      continue;
    }
  }
  // Dummy implementation of getpid
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _getpid()
  {
    return 1;
  }
  // Dummy implementation of kill
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _kill(int, int)
  {
    return -1;
  }
  // Dummy implementation of fstat, makes the assumption that the "device"
  // representing, in this case STDIN, STDOUT, and STDERR as character devices.
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _fstat([[maybe_unused]] int file, struct stat * status)
  {
    status->st_mode = S_IFCHR;
    return 0;
  }
  // Implementation of sbrk for allocating and deallocating space for malloc.
  // NOLINTNEXTLINE(readability-identifier-naming)
  void * _sbrk(int increment)
  {
    static uint8_t * heap_position = &heap;
    void * previous_heap_position  = static_cast<void *>(heap_position);

    // Check that by allocating this space, we do not exceed the heap area.
    if ((heap_position + increment) > &heap_end)
    {
      previous_heap_position = nullptr;
    }
    heap_position += increment;
    return previous_heap_position;
  }
  // Dummy implementation of close
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _close([[maybe_unused]] int file)
  {
    return -1;
  }
  // Minimum implementation of _write using UART0 putchar
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _write([[maybe_unused]] int file, char * ptr, int length)
  {
    for (int i = 0; i < length; i++)
    {
      // TODO(#81): either make this inline, or swap with function that can
      //   take a buffer and length.
      out(ptr[i]);
    }
    return length;
  }
  // Dummy implementation of _lseek
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _lseek([[maybe_unused]] int file, [[maybe_unused]] int ptr,
             [[maybe_unused]] int dir)
  {
    return 0;
  }
  // Minimum implementation of _read using UART0 getchar
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _read(FILE * file, char * ptr, int length)
  {
    if (file == STDIN_FILENO)
    {
      length = 1;
      // TODO(#81): either make this inline
      *ptr = static_cast<char>(in());
      if (*ptr == '\r')
      {
        out('\r');
        *ptr = '\n';
      }
      out(*ptr);
    }
    return length;
  }
  // Needed by third party printf library
  void _putchar(char character)  // NOLINT
  {
    out(character);
  }
  // Needed by third party printf library
  int putchar(int character)  // NOLINT
  {
    return out(character);
  }
  // Overload default nano puts() with a more optimal version of puts
  int puts(const char * str)  // NOLINT
  {
    int i;
    for (i = 0; str[i] != '\0'; i++)
    {
      out(str[i]);
    }
    return i;
  }

  // =============================
  // Backtrace Utility Functions
  // =============================
  void * stack_trace[config::kBacktraceDepth];
  size_t stack_depth = 0;

  void __cyg_profile_func_enter(void *, void * call_site)  // NOLINT
  {
    stack_trace[stack_depth++] = call_site;
  }

  void __cyg_profile_func_exit(void *, void *)  // NOLINT
  {
    stack_depth--;
  }
}

void ** GetStackTrace()
{
  return stack_trace;
}

size_t GetStackDepth()
{
  return stack_depth;
}

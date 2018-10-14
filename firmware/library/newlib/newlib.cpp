#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>
#include <cstdio>

#include "L0_LowLevel/uart0.hpp"
#include "L2_Utilities/macros.hpp"
#include "newlib/newlib.hpp"

constexpr int32_t kHeapSize = 32768;
#if defined(HOST_TEST)
Stdout out = putchar;
Stdin in   = getchar;
uint8_t heap[kHeapSize];
#else
Stdout out = uart0::PutChar;
Stdin in   = uart0::GetChar;
extern uint8_t heap[kHeapSize];
#endif

extern "C"
{
  // Dummy implementation of isatty
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _isatty(int file)
  {
    SJ2_USED(file);
    return 1;
  }
  // Dummy implementation of exit with return code placed into
  // Arm register r3
  // NOLINTNEXTLINE(readability-identifier-naming)
  void _exit(int rc)
  {
    SJ2_USED(rc);
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
  int _fstat(int file, struct stat * status)
  {
    SJ2_USED(file);
    status->st_mode = S_IFCHR;
    return 0;
  }
  // Implementation of sbrk for allocating and deallocating space for malloc.
  // NOLINTNEXTLINE(readability-identifier-naming)
  void * _sbrk(int increment)
  {
    static uint8_t * heap_end = heap;
    void * previous_heap_end  = static_cast<void *>(heap_end);

    // Check that by allocating this space, we do not exceed the heap area.
    if ((heap_end + increment) - heap > kHeapSize)
    {
      previous_heap_end = nullptr;
    }
    heap_end += increment;
    return previous_heap_end;
  }
  // Dummy implementation of close
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _close(int file)
  {
    SJ2_USED(file);
    return -1;
  }
  // Minimum implementation of _write using UART0 putchar
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _write(int file, char * ptr, int length)
  {
    SJ2_USED(file);
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
  int _lseek(int file, int ptr, int dir)
  {
    SJ2_USED(file);
    SJ2_USED(ptr);
    SJ2_USED(dir);
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
}

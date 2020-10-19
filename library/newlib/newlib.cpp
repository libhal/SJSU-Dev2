#include "newlib/newlib.hpp"

#include <sys/stat.h>
#include <unistd.h>

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "L0_Platform/ram.hpp"
#include "third_party/semihost/trace.h"
#include "utility/ansi_terminal_codes.hpp"
#include "utility/error_handling.hpp"
#include "utility/macros.hpp"

extern "C"
{
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
    void * previous_heap_position = static_cast<void *>(heap_position);
    // Check that by allocating this space, we do not exceed the heap area.
    if ((heap_position + increment) > &heap_end)
    {
      previous_heap_position = nullptr;
    }
    heap_position += increment;
    return previous_heap_position;
  }

  // NOLINTNEXTLINE(readability-identifier-naming)
  int _write([[maybe_unused]] int file, const char * ptr, int length)
  {
    trace_write(ptr, length);
    return sjsu::newlib::out(std::span<const char>(ptr, length));
  }

  // NOLINTNEXTLINE(readability-identifier-naming)
  int _read(FILE * file, char * ptr, [[maybe_unused]] int length)
  {
    int number_of_read_characters = 0;
    if (file == STDIN_FILENO)
    {
      number_of_read_characters = sjsu::newlib::in(std::span<char>(ptr, 1));
      // Echo back to STDOUT
      if (sjsu::newlib::echo_back_is_enabled)
      {
        sjsu::newlib::out(std::span<const char>(ptr, 1));
      }
    }
    return number_of_read_characters;
  }

  // Overload default libnano putchar() with a more optimal version that does
  // not use dynamic memory
  int putchar(int character)  // NOLINT
  {
    char character_value = static_cast<char>(character);
    return _write(0, &character_value, 1);
  }

  // Overload default libnano puts() with a more optimal version that does
  // not use dynamic memory
  int puts(const char * str)  // NOLINT
  {
    int string_length = static_cast<int>(strlen(str));
    int result        = 0;

    result += _write(0, str, string_length);
    result += _write(0, "\n", 1);

    return result;
  }

  // Overload default libnano puts() with a more optimal version that does
  // not use dynamic memory
  int fputs(const char * str, FILE * file)  // NOLINT
  {
    int string_length    = static_cast<int>(strlen(str));
    int result           = 0;
    intptr_t file_intptr = reinterpret_cast<intptr_t>(file);
    int file_int         = static_cast<int>(file_intptr);

    result += _write(static_cast<int>(file_int), str, string_length);
    return result;
  }

  // Dummy implementation of _lseek
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _lseek_r([[maybe_unused]] int file,
               [[maybe_unused]] int ptr,
               [[maybe_unused]] int dir)
  {
    return 0;
  }

  // Dummy implementation of close
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _close_r([[maybe_unused]] int file)
  {
    return -1;
  }

  // Dummy implementation of isatty
  // NOLINTNEXTLINE(readability-identifier-naming)
  int _isatty_r([[maybe_unused]] int file)
  {
    return 1;
  }

  void HandleExceptionPointer(std::exception_ptr exception_pointer)
  {
    printf(SJ2_BACKGROUND_RED "Uncaught exception: ");

    try
    {
      if (exception_pointer)
      {
        std::rethrow_exception(exception_pointer);
      }
    }
    catch (const std::exception & e)
    {
      printf("std::exception(%s)\n", e.what());
    }
    catch (sjsu::Exception & e)
    {
      e.Print();
    }
    catch (...)
    {
      printf("Unknown...\n");
    }

    puts(SJ2_COLOR_RESET);
  }
  // Dummy implementation of exit with return code placed into
  // Arm register r3
  // NOLINTNEXTLINE(readability-identifier-naming)
  void _exit(int rc)
  {
    printf("Exit with code: %d\n", rc);
    HandleExceptionPointer(std::current_exception());
    while (1)
    {
      continue;
    }
  }
}

#pragma GCC diagnostic ignored "-Wformat-nonliteral"
// The newlib nano version of scanf relies on malloc.
// Overriding scanf with an efficient static memory variant.
// NOLINTNEXTLINE(readability-identifier-naming)
int scanf(const char * format, ...)
{
  va_list args;
  va_start(args, format);
  int items = vfscanf(stdin, format, args);
  va_end(args);
  return items;
}
#pragma GCC diagnostic warning "-Wformat-nonliteral"

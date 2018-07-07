#include <sys/stat.h>
#include <unistd.h>

#include <cstdio>
#include <cstdint>

#include "L0_LowLevel/uart0.min.hpp"
#include "L2_Utilities/macros.hpp"

// Dummy implementation of isatty
extern "C" int _isatty(int file)
{
    SJ2_USED(file);
    return 1;
}
// Dummy implementation of exit with return code placed into
// Arm register r3
extern "C" void _exit(int rc)
{
    register int t1 asm("r3") = rc;
    SJ2_USED(t1);
    while (1) { continue; }
}
// Dummy implementation of getpid
extern "C" int _getpid()
{
    return 1;
}
// Dummy implementation of kill
extern "C" int _kill(int, int)
{
    return -1;
}
// Dummy implementation of fstat, makes the assumption that the "device"
// representing, in this case STDIN, STDOUT, and STDERR as character devices.
extern "C" int _fstat(int file, struct stat * status)
{
    SJ2_USED(file);
    status->st_mode = S_IFCHR;
    return 0;
}
// Implementation of sbrk for allocating and deallocating space for malloc.
extern "C" void * _sbrk(int increment)
{
    constexpr int32_t kHeapSize = 32768;
    extern uint8_t _heap[kHeapSize];
    static uint8_t * heap_end = _heap;
    void * previous_heap_end  = static_cast<void *>(heap_end);

    // Check that by allocating this space, we do not exceed the heap area.
    if ((heap_end + increment) - _heap > kHeapSize)
    {
        previous_heap_end = nullptr;
    }
    heap_end += increment;
    previous_heap_end = previous_heap_end;
    return previous_heap_end;
}
// Dummy implementation of close
extern "C" int _close(int file)
{
    SJ2_USED(file);
    return -1;
}
// Minimum implementation of _write using UART0 putchar
extern "C" int _write(int file, char * ptr, int length)
{
    SJ2_USED(file);
    for (int i = 0; i < length; i++)
    {
        // TODO(#81): either make this inline, or swap with function that can
        //   take a buffer and length.
        uart0_putchar(ptr[i]);
    }
    return length;
}
// Dummy implementation of _lseek
extern "C" int _lseek(int file, int ptr, int dir)
{
    SJ2_USED(file);
    SJ2_USED(ptr);
    SJ2_USED(dir);
    return 0;
}
// Minimum implementation of _read using UART0 getchar
extern "C" int _read(FILE * file, char * ptr, int length)
{
    if (file == STDIN_FILENO)
    {
        length = 1;
        // TODO(#81): either make this inline
        *ptr = uart0_getchar(0);
        if (*ptr == '\r')
        {
            uart0_putchar('\r');
            *ptr = '\n';
        }
        uart0_putchar(*ptr);
    }
    return length;
}

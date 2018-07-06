#include <cstdio>
#include <cstdarg>

#include "L0_LowLevel/uart0.min.hpp"

// Overriding printf to supply a static memory .text efficent varient.
int printf(const char * fmt, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    int length = vsprintf(buffer, fmt, args);
    va_end(args);

    uart0_puts(buffer);
    return length;
}

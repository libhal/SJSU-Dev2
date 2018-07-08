#include <cstdio>
#include <cstdarg>

#include "L0_LowLevel/uart0.min.hpp"

// Overriding printf to supply a static memory .text efficent varient.
int scanf(const char * fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int items = vfscanf(stdin, fmt, args);
    va_end(args);
    return items;
}

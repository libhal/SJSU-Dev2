#include <cstdio>
#include <cstdarg>

#include "L0_LowLevel/uart0.min.hpp"

// Overriding printf to supply a static memory .text efficient variant.
int printf(const char * fmt, ...)
{
    constexpr size_t kPrintfBufferSize = 256;
    char buffer[kPrintfBufferSize];
    va_list args;
    va_start(args, fmt);
    int length = vsnprintf(buffer, kPrintfBufferSize, fmt, args);
    va_end(args);

    uart0_puts(buffer);
    return length;
}

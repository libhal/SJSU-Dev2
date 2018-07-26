#include <cstdarg>
#include <cstdio>

#include "L0_LowLevel/uart0.hpp"

// Overriding printf to supply a static memory .text efficient variant.
// NOLINTNEXTLINE(readability-identifier-naming)
int printf(const char * format, ...)
{
    constexpr size_t kPrintfBufferSize = 256;
    char buffer[kPrintfBufferSize];
    va_list args;
    va_start(args, format);
    int length = vsnprintf(buffer, kPrintfBufferSize, format, args);
    va_end(args);

    uart0::Puts(buffer);
    return length;
}

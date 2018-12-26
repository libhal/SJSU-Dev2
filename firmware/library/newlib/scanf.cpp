#include <cstdarg>
#include <cstdio>

#pragma GCC diagnostic ignored "-Wformat-nonliteral"
// nano.spec version of scanf relies on malloc.
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

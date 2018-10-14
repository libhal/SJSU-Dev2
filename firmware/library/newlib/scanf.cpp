#include <cstdarg>
#include <cstdio>

// Overriding printf to supply a static memory .text efficient variant.
// NOLINTNEXTLINE(readability-identifier-naming)
int scanf(const char * format, ...)
{
  va_list args;
  va_start(args, format);
  int items = vfscanf(stdin, format, args);
  va_end(args);
  return items;
}

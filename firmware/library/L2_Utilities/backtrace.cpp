#include <unwind.h>
#include <cstdio>

// This log function can be swapped out during runtime, but was mostly meant for
// unit testing.
int (*log_function)(const char *, ...) = printf;

_Unwind_Reason_Code Trace(_Unwind_Context * context, void * depth)
{
  int * depth_value = reinterpret_cast<int *>(depth);
  log_function("    #%d: program counter at %p\n", *depth_value,
               reinterpret_cast<void *>(_Unwind_GetIP(context) - 4));
  (*depth_value)++;
  return _URC_NO_REASON;
}

void PrintTrace()
{
  int depth = 0;
  _Unwind_Backtrace(&Trace, &depth);
}

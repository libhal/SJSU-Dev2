#pragma once

#include <unwind.h>

extern int (*log_function)(const char *, ...);

_Unwind_Reason_Code Trace(_Unwind_Context * context, void * depth);
void PrintTrace();

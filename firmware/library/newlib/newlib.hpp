#pragma once

#include "L2_Utilities/macros.hpp"

using Stdout = int (*)(int);
extern Stdout out;

using Stdin = int (*)();
extern Stdin in;

// NOLINTNEXTLINE(readability-identifier-naming)
extern "C" int _write(int file, char * ptr, int length);

extern "C" void * stack_trace[config::kBacktraceDepth];
extern "C" size_t stack_depth;
// Not ignoring the profile functions within the stack trace will result in
// an recursive loop.
extern "C"
SJ2_IGNORE_STACK_TRACE(void __cyg_profile_func_enter(void*, void*));  // NOLINT
extern "C"
SJ2_IGNORE_STACK_TRACE(void __cyg_profile_func_exit(void*, void*));  // NOLINT

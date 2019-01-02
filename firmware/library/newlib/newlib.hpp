#pragma once

#include <cstring>

#include "utility/macros.hpp"

using Stdout = int (*)(int);
extern Stdout out;

using Stdin = int (*)();
extern Stdin in;

void ** GetStackTrace();
size_t GetStackDepth();

// Not ignoring the profile functions within the stack trace will result in
// an recursive loop.
extern "C"
SJ2_IGNORE_STACK_TRACE(void __cyg_profile_func_enter(void*, void*));  // NOLINT
extern "C"
SJ2_IGNORE_STACK_TRACE(void __cyg_profile_func_exit(void*, void*));  // NOLINT

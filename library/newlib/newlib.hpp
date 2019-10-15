#pragma once

#include <cstddef>

namespace sjsu
{
namespace newlib
{
using Stdout = int (*)(const char *, size_t);
using Stdin  = int (*)(char *, size_t);

void SetStdout(Stdout);
void SetStdin(Stdin);
/// Enables echo back when _read (stdin) is called.
///
/// @param enable_echo - If true, enable echo, if false disable echo back.
void StdinEchoBack(bool enable_echo);
int DoNothingStdOut(const char *, size_t);
int DoNothingStdIn(char *, size_t);

inline bool echo_back_is_enabled = true;
inline Stdout out = DoNothingStdOut;
inline Stdin in   = DoNothingStdIn;
}  // namespace newlib
}  // namespace sjsu

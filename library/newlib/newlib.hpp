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
}  // namespace newlib
}  // namespace sjsu

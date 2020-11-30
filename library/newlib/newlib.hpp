#pragma once

#include <cstddef>
#include <functional>
#include <span>

namespace sjsu
{
namespace newlib
{
using Stdout = std::function<int(std::span<const char>)>;
using Stdin  = std::function<int(std::span<char>)>;

void SetStdout(Stdout);
void SetStdin(Stdin);
/// Enables echo back when _read (stdin) is called.
///
/// @param enable_echo - If true, enable echo, if false disable echo back.
void StdinEchoBack(bool enable_echo);

int DoNothingStdOut(std::span<const char> ignore);
int DoNothingStdIn(std::span<char> ignore);

inline bool echo_back_is_enabled = true;
inline Stdout out                = DoNothingStdOut;
inline Stdin in                  = DoNothingStdIn;
}  // namespace newlib
}  // namespace sjsu

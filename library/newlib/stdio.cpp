#include "newlib/newlib.hpp"

namespace sjsu
{
namespace newlib
{
int DoNothingStdOut(std::span<const char> ignore)
{
  return ignore.size();
}
int DoNothingStdIn(std::span<char> ignore)
{
  return ignore.size();
}
void SetStdout(Stdout stdout_handler)
{
  out = stdout_handler;
}
void SetStdin(Stdin stdin_handler)
{
  in = stdin_handler;
}
void StdinEchoBack(bool enable_echo)
{
  echo_back_is_enabled = enable_echo;
}
}  // namespace newlib
}  // namespace sjsu

// Needed by third party "printf" library
extern "C" void _putchar(char character)  // NOLINT
{
  sjsu::newlib::out(std::span<char>(&character, 1));
}

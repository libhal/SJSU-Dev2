#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "config.hpp"
#include "utility/debug.hpp"
#include "utility/log.hpp"

int Baz(void)
{
  sjsu::LogInfo(
      "Calling backtrace application directly within Baz() function...");
  sjsu::LogInfo("Each line shows the address of the function that was called.");
  sjsu::LogInfo(
      "The 0th address is the first function that was called from main.");
  sjsu::LogInfo("The last address is where the stack trace was called from.");
  sjsu::LogInfo(
      "Everything in the middle is the sequence of calls that got you to this "
      "point.\n");
  sjsu::debug::PrintBacktrace();
  puts("\n");
  return 2;
}

int Bar(void)
{
  Baz();

  SJ2_ASSERT_FATAL(
      false,
      "SJ2_ASSERT_FATAL will cause the system to abort operations. This "
      "should be used in cases where a users action or an improper setup "
      "could lead to hardware damage or sever maloperation.");

  return -1;
}

int Foo(void)
{
  return Bar();
}

int main()
{
  // This application assumes that all pins are set to function 0 (GPIO)
  sjsu::LogInfo("Backtrace Application Starting...");

  Foo();

  // This will never be reached
  return 0;
}

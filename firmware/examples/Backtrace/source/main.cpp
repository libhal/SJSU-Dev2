#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "config.hpp"
#include "utility/debug.hpp"
#include "utility/log.hpp"

int Baz(void)
{
  LOG_INFO("Calling backtrace application directly within Baz() function...");
  LOG_INFO("Each line shows the address of the function that was called.");
  LOG_INFO("The 0th address is the first function that was called from main.");
  LOG_INFO("The last address is where the stack trace was called from.");
  LOG_INFO(
      "Everything in the middle is the sequence of calls that got you to this "
      "point.\n");
  debug::PrintBacktrace();
  puts("\n");
  return 2;
}

int Bar(void)
{
  Baz();

  SJ2_ASSERT_WARNING(2 < 5,
                     "This assert warning message shouldn't show up "
                     "since the condition evaluates to true. If it wasn't "
                     "true, then this message would be printed to STDOUT. ");
  SJ2_ASSERT_WARNING(1 == 0,
                     "This assert warning message SHOULD show up, since the "
                     "condition is, does 1 == 0, which is false. Since this "
                     "is just a warning, the program will be stoped.");
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

int main(void)
{
  // This application assumes that all pins are set to function 0 (GPIO)
  LOG_INFO("Backtrace Application Starting...");
  Foo();
  return 0;
}

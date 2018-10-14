#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "config.hpp"
#include "L2_Utilities/macros.hpp"

int Baz(void)
{
  DEBUG_PRINT(
      "Calling backtrace application directly within Baz() function...");
  SJ2_DUMP_BACKTRACE();
  return 2;
}

int Bar(void)
{
  return Baz();
}

int Foo(void)
{
  return Bar();
}

int main(void)
{
  // This application assumes that all pins are set to function 0 (GPIO)
  DEBUG_PRINT("Backtrace Application Starting...");
  Foo();
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
  return 0;
}

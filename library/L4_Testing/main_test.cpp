#include <unistd.h>

#include "newlib/newlib.hpp"
#define DOCTEST_CONFIG_IMPLEMENT
#include "testing_frameworks.hpp"

DEFINE_FFF_GLOBALS

int HostWrite(const char * payload, size_t length)
{
  return static_cast<int>(write(1, payload, length));
}
int HostRead(char * payload, size_t length)
{
  return static_cast<int>(read(1, payload, length));
}

int main(int argc, char * argv[])
{
  doctest::Context context;

  sjsu::newlib::SetStdout(HostWrite);
  sjsu::newlib::SetStdin(HostRead);

  context.applyCommandLine(argc, argv);

  int res = context.run();  // run

  // important - query flags (and --exit) rely on the user doing this propagate
  // the result of the tests
  if (context.shouldExit())
  {
    return res;
  }

  int client_stuff_return_code = 0;
  // your program - if the testing framework is integrated in your production
  // code

  return res + client_stuff_return_code;  // the result from doctest is
                                          // propagated here as well
}

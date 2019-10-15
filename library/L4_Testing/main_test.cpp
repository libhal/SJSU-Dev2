// This tells Catch to provide a main() - only do this in one cpp file
#define CATCH_CONFIG_RUNNER
#include <unistd.h>

#include "newlib/newlib.hpp"
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
  sjsu::newlib::SetStdout(HostWrite);
  sjsu::newlib::SetStdin(HostRead);

  int result = Catch::Session().run(argc, argv);

  return result;
}

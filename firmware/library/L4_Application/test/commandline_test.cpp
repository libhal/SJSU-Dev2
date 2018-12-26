#include "L4_Application/commandline.hpp"
#include "L5_Testing/testing_frameworks.hpp"

namespace
{
[[maybe_unused]] CommandList_t<5> command_list;
[[maybe_unused]] CommandLine<command_list> command_line;
}  // namespace

TEST_CASE("CommandLine Test", "[commandline]")
{
  SECTION("Initialize")
  {
  }
}

EMIT_ALL_METHODS(Command);

TEST_CASE("Command Test", "[command]")
{
  SECTION("Initialize")
  {
  }
}

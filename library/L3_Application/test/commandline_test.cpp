#include "L3_Application/commandline.hpp"

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Command);

namespace
{
FAKE_VALUE_FUNC(int, fake_command, int, const char * const *);
constexpr char kTestCommandName[]        = "test_command";
constexpr char kTestCommandDescription[] = "A test command description";
}  // namespace

TEST_CASE("Command Test")
{
  Command test_command(kTestCommandName, kTestCommandDescription, fake_command);
  SECTION("Get Command Name")
  {
    CHECK(strcmp(kTestCommandName, test_command.GetName()) == 0);
  }
  SECTION("Get Command Description")
  {
    CHECK(strcmp(kTestCommandDescription, test_command.GetDescription()) == 0);
  }
  SECTION("Execute Program")
  {
    fake_command_fake.return_val = 0xFF;
    const char * test_args[]     = { "Hello", "World" };

    CHECK(0xFF == test_command.Program(2, test_args));

    CHECK(fake_command_fake.call_count == 1);
    CHECK(fake_command_fake.arg0_val == 2);
    CHECK(fake_command_fake.arg1_val == test_args);
  }
}

namespace
{
[[maybe_unused]] CommandList_t<5> command_list;
[[maybe_unused]] CommandLine<command_list> command_line;
}  // namespace

TEST_CASE("CommandLine Test")
{
  SECTION("AddCommand")
  {
    // command_line.AddCommand(&test_command);
  }
}
}  // namespace sjsu

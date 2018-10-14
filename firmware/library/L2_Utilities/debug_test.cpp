#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "L2_Utilities/debug.hpp"
#include "L5_Testing/testing_frameworks.hpp"
#include "newlib/newlib.hpp"

namespace
{
char memory_out[512];
int memory_out_position = 0;
int TestStandardOutput(int out_char)
{
  memory_out[memory_out_position++] = static_cast<char>(out_char);
  return 1;
}
void ResetTestStdoutBuffer()
{
  memory_out_position = 0;
  memset(memory_out, 0x00, sizeof(memory_out));
}
}  // namespace

TEST_CASE("Testing Debug Utilities", "[hexdump]")
{
  // Saving previous out to be restored after test
  Stdout previous_out = out;
  // Inject "TestStandardOutput" as new stdout function.
  out = TestStandardOutput;

  SECTION("Hex dump byte array 16 byte aligned")
  {
    ResetTestStdoutBuffer();

    uint8_t memory[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                         0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    constexpr char kExpected[] =
        "00000000  00 11 22 33 44 55 66 77  88 99 AA BB CC DD EE FF  "
        "|..\"3DUfw........|\n"
        "00000010  \n";

    debug::Hexdump(memory, sizeof(memory));

    CHECK_THAT(memory_out, Catch::Matchers::Equals(kExpected));
  }

  SECTION("Hex dump string with \"hello!\", not aligned with 16")
  {
    ResetTestStdoutBuffer();

    char memory[] = "hello!\n";
    constexpr char kExpected[] =
        "00000000  68 65 6C 6C 6F 21 0A                              "
        "|hello!.|\n"
        "00000008  \n";

    debug::Hexdump(memory, sizeof(memory));

    CHECK_THAT(memory_out, Catch::Matchers::Equals(kExpected));
  }

  SECTION("Hex dump string with two rows 16 aligned")
  {
    ResetTestStdoutBuffer();

    uint8_t memory[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                         0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,
                         0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78,
                         0x89, 0x9A, 0xAB, 0xBC, 0xCD, 0xDE, 0xEF, 0xF0 };
    constexpr char kExpected[] =
        "00000000  00 11 22 33 44 55 66 77  88 99 AA BB CC DD EE FF  "
        "|..\"3DUfw........|\n"
        "00000010  01 12 23 34 45 56 67 78  89 9A AB BC CD DE EF F0  "
        "|..#4EVgx........|\n"
        "00000020  \n";

    debug::Hexdump(memory, sizeof(memory));

    CHECK_THAT(memory_out, Catch::Matchers::Equals(kExpected));
  }
  // Restore stdout function
  out = previous_out;
}

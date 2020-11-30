#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include "L4_Testing/testing_frameworks.hpp"
#include "newlib/newlib.hpp"
#include "utility/debug.hpp"

namespace sjsu
{
TEST_CASE("Testing Debug Utilities")
{
  using newlib::Stdout;

  char debug_memory_out[512]    = { 0 };
  int debug_memory_out_position = 0;

  sjsu::newlib::SetStdout([&debug_memory_out, &debug_memory_out_position](
                              std::span<const char> out_char) {
    for (size_t i = 0; i < out_char.size(); i++)
    {
      debug_memory_out[debug_memory_out_position++] = out_char[i];
    }
    return 1;
  });

  SECTION("Hex dump byte array 16 byte aligned")
  {
    uint8_t memory[] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
                         0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    constexpr char kExpected[] =
        "00000000  00 11 22 33 44 55 66 77  88 99 AA BB CC DD EE FF  "
        "|..\"3DUfw........|\n"
        "00000010  \n";

    sjsu::debug::Hexdump(memory, sizeof(memory));

    for (size_t i = 0; i < sizeof(kExpected); i++)
    {
      CAPTURE(i);
      CHECK(debug_memory_out[i] == kExpected[i]);
    }
  }

  SECTION("Hex dump string with \"hello!\", not aligned with 16")
  {
    char memory[] = "hello!\n";
    constexpr char kExpected[] =
        "00000000  68 65 6C 6C 6F 21 0A 00                           "
        "|hello!..|\n"
        "00000008  \n";

    sjsu::debug::Hexdump(memory, sizeof(memory));

    for (size_t i = 0; i < sizeof(kExpected); i++)
    {
      CAPTURE(i);
      CHECK(debug_memory_out[i] == kExpected[i]);
    }
  }

  SECTION("Hex dump string with two rows 16 aligned")
  {
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

    sjsu::debug::Hexdump(memory, sizeof(memory));

    for (size_t i = 0; i < sizeof(kExpected); i++)
    {
      CAPTURE(i);
      CHECK(debug_memory_out[i] == kExpected[i]);
    }
  }

  // Restore stdout function
  newlib::SetStdout(HostTestWrite);
}  // namespace sjsu
}  // namespace sjsu

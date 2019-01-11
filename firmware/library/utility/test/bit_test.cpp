#include <cstdint>

#include "L4_Testing/testing_frameworks.hpp"
#include "utility/bit.hpp"

TEST_CASE("Testing Bit Manipulations", "[bit manipulation]")
{
  SECTION("Extract")
  {
    // Static_assert is used to make sure that the functions work at compile
    // time.
    CHECK(0b0011 == bit::Extract(0b0000'1111, 2, 4));
    static_assert(0b0011 == bit::Extract(0b0000'1111, 2, 4));

    CHECK(0b0000 == bit::Extract(0b0000'1111, 4, 4));
    static_assert(0b0000 == bit::Extract(0b0000'1111, 4, 4));

    CHECK(0b0001 == bit::Extract(0b0000'1111, 0, 1));
    static_assert(0b0001 == bit::Extract(0b0000'1111, 0, 1));

    CHECK(0x00AA == bit::Extract(0xAA00'0000, 24, 8));
    static_assert(0x00AA == bit::Extract(0xAA00'0000, 24, 8));

    CHECK(0xFFFF == bit::Extract(0xFFFF'FFFF, 15, 16));
    static_assert(0xFFFF == bit::Extract(0xFFFF'FFFF, 15, 16));

    CHECK(0b0001 == bit::Extract(0xFFFF'FFFF, 31, 16));
    static_assert(0b0001 == bit::Extract(0xFFFF'FFFF, 31, 16));

    CHECK(0xBEEF == bit::Extract(0xDEAD'BEEF, 0, 16));
    static_assert(0xBEEF == bit::Extract(0xDEAD'BEEF, 0, 16));

    CHECK(0xDEAD == bit::Extract(0xDEAD'BEEF, 16, 16));
    static_assert(0xDEAD == bit::Extract(0xDEAD'BEEF, 16, 16));

    CHECK(0x00AD == bit::Extract(0xDEAD'BEEF, 16, 8));
    static_assert(0x00AD == bit::Extract(0xDEAD'BEEF, 16, 8));
  }

  SECTION("Insert")
  {
    CHECK(0b0110'0000 == bit::Insert(0, 0b11, 5, 2));
    static_assert(0b0110'0000 == bit::Insert(0, 0b11, 5, 2));

    CHECK(0b0000'1110 == bit::Insert(0, 0b111, 1, 3));
    static_assert(0b0000'1110 == bit::Insert(0, 0b111, 1, 3));

    CHECK(0b0000'1111 == bit::Insert(0, 0b1111, 0, 4));
    static_assert(0b0000'1111 == bit::Insert(0, 0b1111, 0, 4));

    CHECK(0xAB00'0000 == bit::Insert(0, 0xAB, 24, 8));
    static_assert(0xAB00'0000 == bit::Insert(0, 0xAB, 24, 8));

    CHECK(0xDEAD'BEEF == bit::Insert(0xD00D'BEEF, 0xEA, 20, 8));
    static_assert(0xDEAD'BEEF == bit::Insert(0xD00D'BEEF, 0xEA, 20, 8));

    // Shows replacement of DEAD -> BEEF
    CHECK(0xDEAD'BEEF == bit::Insert(0xDEAD'DEAD, 0xBEEF, 0, 16));
    static_assert(0xDEAD'BEEF == bit::Insert(0xDEAD'DEAD, 0xBEEF, 0, 16));

    // Shows replacement of 0101 -> 1111 in the middle of byte
    CHECK(0b1011'1101 == bit::Insert(0b1010'0101, 0b1111, 2, 4));
    static_assert(0b1011'1101 == bit::Insert(0b1010'0101, 0b1111, 2, 4));

    uint32_t target = 0xAAAA'BBBB;
    int8_t value_to_insert = 0xCD;
    // Shows replacement of 0101 -> 1111 in the middle of byte
    CHECK(0xAAAA'BCDB == bit::Insert(target, value_to_insert, 4, 8));
  }

  SECTION("Set")
  {
    CHECK(0b0001'1111 == bit::Set(0b1111, 4));
    static_assert(0b0001'1111 == bit::Set(0b1111, 4));

    CHECK(0xFFFF'FFFF == bit::Set(0x7FFFFFFF, 31));
    static_assert(0xFFFF'FFFF == bit::Set(0x7FFFFFFF, 31));

    CHECK(0b0000'0001 == bit::Set(0, 0));
    static_assert(0b0000'0001 == bit::Set(0, 0));

    CHECK(0b1000'1001 == bit::Set(0b1000'0001, 3));
    static_assert(0b1000'1001 == bit::Set(0b1000'0001, 3));
  }

  SECTION("Clear")
  {
    CHECK(0b0000'1111 == bit::Clear(0b0001'1111, 4));
    static_assert(0b0000'1111 == bit::Clear(0b0001'1111, 4));

    CHECK(0x7FFF'FFFF == bit::Clear(0xFFFF'FFFF, 31));
    static_assert(0x7FFF'FFFF == bit::Clear(0xFFFF'FFFF, 31));

    CHECK(0b0000'0000 == bit::Clear(0b0000'0001, 0));
    static_assert(0b0000'0000 == bit::Clear(0b0000'0001, 0));

    CHECK(0b1000'0001 == bit::Clear(0b1000'1001, 3));
    static_assert(0b1000'0001 == bit::Clear(0b1000'1001, 3));
  }

  SECTION("Toggle")
  {
    CHECK(0b111 == bit::Clear(0b1111, 3));
    static_assert(0b111 == bit::Clear(0b1111, 3));

    CHECK(0xFFFFFFFE == bit::Clear(0xFFFFFFFF, 0));
    static_assert(0xFFFFFFFE == bit::Clear(0xFFFFFFFF, 0));

    CHECK(0x7FFFFFFF == bit::Clear(0XFFFFFFFF, 31));
    static_assert(0x7FFFFFFF == bit::Clear(0XFFFFFFFF, 31));
  }

  SECTION("Read")
  {
    CHECK(true == bit::Read(0b1111, 3));
    static_assert(true == bit::Read(0b1111, 3));

    CHECK(false == bit::Read(0b01111, 4));
    static_assert(false == bit::Read(0b01111, 4));

    CHECK(true == bit::Read(0b1, 0));
    static_assert(true == bit::Read(0b1, 0));

    CHECK(false == bit::Read(0b0, 0));
    static_assert(false == bit::Read(0b0, 0));

    CHECK(true == bit::Read(0x80000000, 31));
    static_assert(true == bit::Read(0x80000000, 31));

    CHECK(false == bit::Read(0x80000000, 5));
    static_assert(false == bit::Read(0x80000000, 5));

    CHECK(false == bit::Read(0x0000, 31));
    static_assert(false == bit::Read(0x0000, 31));
  }
}

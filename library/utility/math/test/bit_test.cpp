#include <cstdint>

#include "testing/testing_frameworks.hpp"
#include "utility/math/bit.hpp"

namespace sjsu
{
TEST_CASE("Testing Bit Manipulations")
{
  SECTION("MaskFromRange()")
  {
    SECTION("(start, end)")
    {
      CHECK(bit::Mask{ .position = 5, .width = 16 - 4 } ==
            bit::MaskFromRange(5, 16));
      static_assert(bit::Mask{ .position = 5, .width = 16 - 4 } ==
                    bit::MaskFromRange(5, 16));

      CHECK(bit::Mask{ .position = 16, .width = 47 - 15 } ==
            bit::MaskFromRange(16, 47));
      static_assert(bit::Mask{ .position = 16, .width = 47 - 15 } ==
                    bit::MaskFromRange(16, 47));

      CHECK(bit::Mask{ .position = 1, .width = 61 } ==
            bit::MaskFromRange(1, 61));
      static_assert(bit::Mask{ .position = 1, .width = 61 } ==
                    bit::MaskFromRange(1, 61));

      CHECK(bit::Mask{ .position = 55, .width = 89 - 54 } ==
            bit::MaskFromRange(55, 89));
      static_assert(bit::Mask{ .position = 55, .width = 89 - 54 } ==
                    bit::MaskFromRange(55, 89));
    }
    SECTION("Single Bit")
    {
      CHECK(bit::Mask{ .position = 5, .width = 1 } == bit::MaskFromRange(5));
      static_assert(bit::Mask{ .position = 5, .width = 1 } ==
                    bit::MaskFromRange(5));

      CHECK(bit::Mask{ .position = 47, .width = 1 } == bit::MaskFromRange(47));
      static_assert(bit::Mask{ .position = 47, .width = 1 } ==
                    bit::MaskFromRange(47));

      CHECK(bit::Mask{ .position = 61, .width = 1 } == bit::MaskFromRange(61));
      static_assert(bit::Mask{ .position = 61, .width = 1 } ==
                    bit::MaskFromRange(61));

      CHECK(bit::Mask{ .position = 7, .width = 1 } == bit::MaskFromRange(7));
      static_assert(bit::Mask{ .position = 7, .width = 1 } ==
                    bit::MaskFromRange(7));
    }
  }

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
    static_assert(0xAB00'0000UL == bit::Insert(0UL, 0xAB, 24, 8));

    CHECK(0xDEAD'BEEF == bit::Insert(0xD00D'BEEF, 0xEA, 20, 8));
    static_assert(0xDEAD'BEEF == bit::Insert(0xD00D'BEEF, 0xEA, 20, 8));

    // Shows replacement of DEAD -> BEEF
    CHECK(0xDEAD'BEEF == bit::Insert(0xDEAD'DEAD, 0xBEEF, 0, 16));
    static_assert(0xDEAD'BEEF == bit::Insert(0xDEAD'DEAD, 0xBEEF, 0, 16));

    // Shows replacement of 0101 -> 1111 in the middle of byte
    CHECK(0b1011'1101 == bit::Insert(0b1010'0101, 0b1111, 2, 4));
    static_assert(0b1011'1101 == bit::Insert(0b1010'0101, 0b1111, 2, 4));

    uint32_t target        = 0xAAAA'BBBB;
    int8_t value_to_insert = 0xCD;
    // Shows replacement of 0101 -> 1111 in the middle of byte
    CHECK(0xAAAA'BCDB == bit::Insert(target, value_to_insert, 4, 8));

    constexpr uint64_t kTest64bit = 0x1001'0000'0000'0000;
    CHECK(0x1001 ==
          bit::Extract(kTest64bit, bit::Mask{ .position = 48, .width = 16 }));
  }

  SECTION("SignedExtract")
  {
    CHECK(-55 == bit::SignedExtract<int16_t>(0x00'FC'90'00,
                                             bit::MaskFromRange(12, 24)));
    static_assert(-55 == bit::SignedExtract<int16_t>(
                             0x00'FC'90'00, bit::MaskFromRange(12, 24)));

    CHECK(-55 == bit::SignedExtract<int32_t>(0x00'FC'90'00,
                                             bit::MaskFromRange(12, 24)));
    static_assert(-55 == bit::SignedExtract<int32_t>(
                             0x00'FC'90'00, bit::MaskFromRange(12, 24)));

    CHECK(-1234 == bit::SignedExtract<int32_t>(0x00'FB'2E'00,
                                               bit::MaskFromRange(8, 24)));
    static_assert(-1234 == bit::SignedExtract<int32_t>(
                               0x00'FB'2E'00, bit::MaskFromRange(8, 24)));

    CHECK(-1234 == bit::SignedExtract<int32_t>(0x00'FB'2E'00 >> 3,
                                               bit::MaskFromRange(8, 24) >> 3));
    static_assert(-1234 ==
                  bit::SignedExtract<int32_t>(0x00'FB'2E'00 >> 3,
                                              bit::MaskFromRange(8, 24) >> 3));

    CHECK(77 == bit::SignedExtract<int8_t>(0x04'D0'00'00,
                                           bit::MaskFromRange(20, 28)));
    static_assert(77 == bit::SignedExtract<int8_t>(0x04'D0'00'00,
                                                   bit::MaskFromRange(20, 28)));
  }

  SECTION("Insert with Mask")
  {
    CHECK(0b0110'0000 == bit::Insert(0, 0b11,
                                     bit::Mask{
                                         .position = 5,
                                         .width    = 2,
                                     }));
    static_assert(0b0110'0000 == bit::Insert(0, 0b11,
                                             bit::Mask{
                                                 .position = 5,
                                                 .width    = 2,
                                             }));

    CHECK(0b0000'1110 == bit::Insert(0, 0b111,
                                     bit::Mask{
                                         .position = 1,
                                         .width    = 3,
                                     }));
    static_assert(0b0000'1110 == bit::Insert(0, 0b111,
                                             bit::Mask{
                                                 .position = 1,
                                                 .width    = 3,
                                             }));

    CHECK(0b0000'1111 == bit::Insert(0, 0b1111,
                                     bit::Mask{
                                         .position = 0,
                                         .width    = 4,
                                     }));
    static_assert(0b0000'1111 == bit::Insert(0, 0b1111,
                                             bit::Mask{
                                                 .position = 0,
                                                 .width    = 4,
                                             }));

    CHECK(0xAB00'0000 == bit::Insert(0, 0xAB,
                                     bit::Mask{
                                         .position = 24,
                                         .width    = 8,
                                     }));
    static_assert(0xAB00'0000UL == bit::Insert(0UL, 0xAB,
                                               bit::Mask{
                                                   .position = 24,
                                                   .width    = 8,
                                               }));

    CHECK(0xDEAD'BEEF == bit::Insert(0xD00D'BEEF, 0xEA,
                                     bit::Mask{
                                         .position = 20,
                                         .width    = 8,
                                     }));
    static_assert(0xDEAD'BEEF == bit::Insert(0xD00D'BEEF, 0xEA,
                                             bit::Mask{
                                                 .position = 20,
                                                 .width    = 8,
                                             }));

    // Shows replacement of DEAD -> BEEF
    CHECK(0xDEAD'BEEF == bit::Insert(0xDEAD'DEAD, 0xBEEF,
                                     bit::Mask{
                                         .position = 0,
                                         .width    = 16,
                                     }));
    static_assert(0xDEAD'BEEF == bit::Insert(0xDEAD'DEAD, 0xBEEF,
                                             bit::Mask{
                                                 .position = 0,
                                                 .width    = 16,
                                             }));

    // Shows replacement of 0101 -> 1111 in the middle of byte
    CHECK(0b1011'1101 == bit::Insert(0b1010'0101, 0b1111,
                                     bit::Mask{
                                         .position = 2,
                                         .width    = 4,
                                     }));
    static_assert(0b1011'1101 == bit::Insert(0b1010'0101, 0b1111,
                                             bit::Mask{
                                                 .position = 2,
                                                 .width    = 4,
                                             }));

    uint32_t target        = 0xAAAA'BBBB;
    int8_t value_to_insert = 0xCD;
    // Shows replacement of 0101 -> 1111 in the middle of byte
    CHECK(0xAAAA'BCDB == bit::Insert(target, value_to_insert,
                                     bit::Mask{
                                         .position = 4,
                                         .width    = 8,
                                     }));

    constexpr uint64_t kTest64bit = 0x1001'0000'0000'0000;
    CHECK(0x1001'ABCD'0000'0000UL ==
          bit::Insert(kTest64bit, 0xABCD,
                      bit::Mask{ .position = 32, .width = 16 }));
  }

  SECTION("Set")
  {
    CHECK(0b0001'1111 == bit::Set(0b1111, 4));
    static_assert(0b0001'1111 == bit::Set(0b1111, 4));

    CHECK(0xFFFF'FFFF == bit::Set(0x7FFFFFFF, 31));
    static_assert(0xFFFF'FFFFUL == bit::Set(0x7FFFFFFFUL, 31));

    CHECK(0b0000'0001 == bit::Set(0, 0));
    static_assert(0b0000'0001 == bit::Set(0, 0));

    CHECK(0b1000'1001 == bit::Set(0b1000'0001, 3));
    static_assert(0b1000'1001 == bit::Set(0b1000'0001, 3));

    constexpr uint64_t kTest64bit = 0x1000'0000'0000'0000;
    CHECK(0x1001'0000'0000'0000UL == bit::Set(kTest64bit, 48));
    static_assert(0x1001'0000'0000'0000UL == bit::Set(kTest64bit, 48));
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

    constexpr uint64_t kTest64bit = 0x1001'0000'0000'0000;
    CHECK(0x1000'0000'0000'0000UL == bit::Clear(kTest64bit, 48));
    static_assert(0x1000'0000'0000'0000UL == bit::Clear(kTest64bit, 48));
  }

  SECTION("Toggle")
  {
    CHECK(0b111 == bit::Clear(0b1111, 3));
    static_assert(0b111 == bit::Clear(0b1111, 3));

    CHECK(0xFFFFFFFE == bit::Clear(0xFFFFFFFF, 0));
    static_assert(0xFFFFFFFE == bit::Clear(0xFFFFFFFF, 0));

    CHECK(0x7FFFFFFF == bit::Clear(0XFFFFFFFF, 31));
    static_assert(0x7FFFFFFF == bit::Clear(0XFFFFFFFF, 31));

    constexpr uint64_t kTest64bit = 0x1001'0000'0000'0000;
    CHECK(0x1003'0000'0000'0000UL == bit::Toggle(kTest64bit, 49));
    static_assert(0x1003'0000'0000'0000UL == bit::Toggle(kTest64bit, 49));
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

    constexpr uint64_t kTest64bit = 0x0001'0000'0000'0000;

    CHECK(true == bit::Read(kTest64bit, 48));
    static_assert(true == bit::Read(kTest64bit, 48));

    CHECK(false == bit::Read(kTest64bit, 49));
    static_assert(false == bit::Read(kTest64bit, 49));
  }

  SECTION("StreamExtract")
  {
    SECTION("Little Endian")
    {
      SECTION("32-bit value in a 128 bit (16 byte) stream")
      {
        // Setup
        std::array<uint8_t, 16> test = {
          0x00, 0x00, 0x00, 0x00, 0xDE, 0xAD, 0xBE, 0xEF,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };

        constexpr uint32_t kExpectedBase = 0xDEADBEEF;

        for (int i = 0; i < 16; i++)
        {
          uint32_t current_expected = kExpectedBase >> i;
          bit::Mask mask            = {
            .position = static_cast<uint8_t>(64 + i),
            .width    = static_cast<uint8_t>(32 - i),
          };
          INFO("Failure on index [" << i << "]");
          CHECK(current_expected == bit::StreamExtract<uint32_t>(test, mask));
        }

        for (int i = 0; i < 16; i++)
        {
          uint32_t current_expected = kExpectedBase & (0xFF'FF'FF'FF >> i);
          bit::Mask mask            = {
            .position = static_cast<uint8_t>(64),
            .width    = static_cast<uint8_t>(32 - i),
          };
          INFO("Failure on index [" << i << "]");
          CHECK(current_expected == bit::StreamExtract<uint32_t>(test, mask));
        }
      }

      SECTION("8 bit value in a single byte stream")
      {
        std::array<uint8_t, 1> test = { 0b0011'1100 };
        bit::Mask mask              = bit::MaskFromRange(2, 5);
        CHECK(0b1111 == bit::StreamExtract<uint8_t>(test, mask));
      }

      SECTION("8 bit value that crosses two bytes")
      {
        std::array<uint8_t, 2> test = { 0b0000'0111, 0b1110'0000 };
        bit::Mask mask              = { .position = 4, .width = 8 };
        CHECK(0b0111'1110 == bit::StreamExtract<uint32_t>(test, mask));
      }

      SECTION("Real Life Example")
      {
        // Example: CSD register for SanDisk 8 GB Micro SD card
        constexpr std::array<uint8_t, 16> kCSD = {
          /* [0] = */ 0x40,  /* [1] = */ 0x0E,  /* [2] = */ 0x00,
          /* [3] = */ 0x32,  /* [4] = */ 0x5B,
          /* [5] = */ 0x59,  /* [6] = */ 0x00,  /* [7] = */ 0x00,
          /* [8] = */ 0x3B,  /* [9] = */ 0x37,
          /* [10] = */ 0x7F, /* [11] = */ 0x80, /* [12] = */ 0x0A,
          /* [13] = */ 0x40, /* [14] = */ 0x40,
          /* [15] = */ 0xAF,
        };

        // Bits in SD card driver are [48:69]
        constexpr bit::Mask kCSizeMask = bit::MaskFromRange(48, 69);
        // C_SIZE should equal 15159 & 0x3b37
        constexpr uint32_t kCSizeBytes = kCSD[7] << 16 | kCSD[8] << 8 | kCSD[9];
        constexpr uint32_t kExpected =
            bit::Extract(kCSizeBytes, { .position = 0, .width = 21 });

        INFO("position = " << static_cast<int>(kCSizeMask.position)
                           << " :: width = "
                           << static_cast<int>(kCSizeMask.width));

        CHECK(kExpected == bit::StreamExtract<uint32_t>(kCSD, kCSizeMask));
      }
    }

    SECTION("Big Endian")
    {
      SECTION("32-bit value in a 128 bit (16 byte) stream")
      {
        // Setup
        std::array<uint8_t, 16> test = {
          0x00, 0x00, 0x00, 0x00, 0xEF, 0xBE, 0xAD, 0xDE,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        };

        constexpr uint32_t kExpectedBase = 0xDEADBEEF;

        for (int i = 0; i < 16; i++)
        {
          uint32_t current_expected = kExpectedBase >> i;
          bit::Mask mask            = {
            .position = static_cast<uint8_t>(32 + i),
            .width    = static_cast<uint8_t>(32 - i),
          };
          INFO("Failure on index [" << i << "]");
          CHECK(current_expected ==
                bit::StreamExtract<uint32_t>(test, mask, Endian::kBig));
        }

        for (int i = 0; i < 16; i++)
        {
          uint32_t current_expected = kExpectedBase & (0xFF'FF'FF'FF >> i);
          bit::Mask mask            = {
            .position = static_cast<uint8_t>(32),
            .width    = static_cast<uint8_t>(32 - i),
          };
          INFO("Failure on index [" << i << "]");
          CHECK(current_expected ==
                bit::StreamExtract<uint32_t>(test, mask, Endian::kBig));
        }
      }

      SECTION("8 bit value in a single byte stream")
      {
        std::array<uint8_t, 1> test = { 0b0011'1100 };
        bit::Mask mask              = bit::MaskFromRange(2, 5);
        CHECK(0b1111 == bit::StreamExtract<uint8_t>(test, mask, Endian::kBig));
      }

      SECTION("8 bit value that crosses two bytes")
      {
        std::array<uint8_t, 2> test = { 0b1110'0000, 0b0000'0111 };
        bit::Mask mask              = { .position = 4, .width = 8 };
        CHECK(0b0111'1110 ==
              bit::StreamExtract<uint32_t>(test, mask, Endian::kBig));
      }
    }
  }
}
}  // namespace sjsu

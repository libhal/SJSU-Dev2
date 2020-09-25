#include "L1_Peripheral/uart.hpp"

#include <algorithm>
#include <array>

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing L1 uart")
{
  Mock<Uart> mock_uart;
  Fake(OverloadedMethod(mock_uart, Write, void(std::span<const uint8_t>)));
  Fake(OverloadedMethod(mock_uart, Read, size_t(std::span<uint8_t>)));
  Fake(Method(mock_uart, HasData));
  Uart & uart = mock_uart.get();

  SECTION("Write() Byte")
  {
    // Setup
    constexpr uint8_t kExpectedByte = 0x3F;
    uint8_t actual_byte             = 0;

    When(OverloadedMethod(mock_uart, Write, void(std::span<const uint8_t>)))
        .Do([&actual_byte](std::span<const uint8_t> data) -> void {
          REQUIRE(1 == data.size());
          actual_byte = data[0];
        });

    // Exercise
    uart.Write(kExpectedByte);

    // Verify
    CHECK(kExpectedByte == actual_byte);
  }

  SECTION("Write() initializer_list<>")
  {
    // Setup
    bool write_was_called = false;
    When(OverloadedMethod(mock_uart, Write, void(std::span<const uint8_t>)))
        .Do([&write_was_called](std::span<const uint8_t> data) -> void {
          CHECK(5 == data.size());

          CHECK(0x12 == data[0]);
          CHECK(0x45 == data[1]);
          CHECK(0xA7 == data[2]);
          CHECK(0xA2 == data[3]);
          CHECK(0x55 == data[4]);

          write_was_called = true;
        });

    // Exercise
    uart.Write({ 0x12, 0x45, 0xA7, 0xA2, 0x55 });

    // Verify
    CHECK(write_was_called);
  }

  SECTION("Read() Byte")
  {
    // Setup
    constexpr uint8_t kExpectedByte = 0x58;

    When(OverloadedMethod(mock_uart, Read, size_t(std::span<uint8_t>)))
        .Do([kExpectedByte](std::span<uint8_t> data) -> size_t {
          REQUIRE(1 == data.size());
          data[0] = kExpectedByte;
          return 1;
        });

    // Exercise
    uint8_t actual_byte = uart.Read();

    // Verify
    CHECK(kExpectedByte == actual_byte);
  }

  SECTION("PollingFlush()")
  {
    // Setup
    When(Method(mock_uart, HasData))
        .Return(true)
        .Return(true)
        .Return(true)
        .Return(false);

    // Exercise
    uart.PollingFlush();

    // Verify
    Verify(Method(mock_uart, HasData)).Exactly(4);
    Verify(OverloadedMethod(mock_uart, Read, size_t(std::span<uint8_t>)))
        .Exactly(3);
  }

  SECTION("Read() with timeout")
  {
    // Setup
    int read_position                                              = 0;
    int return_flag_position                                       = 0;
    static constexpr size_t kPayloadLength                         = 4;
    static constexpr std::array<uint8_t, kPayloadLength> kResponse = {
      0xAA, 0xBB, 0xCC, 0xDD
    };
    static constexpr std::array<bool, 7> kReturnByteFlag = {
      true, true, true, false, false, false, true,
    };

    std::array<uint8_t, kPayloadLength> bytes;
    std::fill(bytes.begin(), bytes.end(), 0x00);

    When(OverloadedMethod(mock_uart, Read, size_t(std::span<uint8_t>)))
        .AlwaysDo([&read_position,
                   &return_flag_position](std::span<uint8_t> data) -> size_t {
          if (kReturnByteFlag[return_flag_position++])
          {
            data[0] = kResponse[read_position++];
            return 1;
          }
          else
          {
            return 0;
          }
        });

    // Exercise
    auto bytes_read = uart.Read(bytes, 10us);

    // Verify
    CHECK(kPayloadLength == bytes_read);
    CHECK(kPayloadLength == read_position);
    CHECK(kResponse == bytes);
    Verify(OverloadedMethod(mock_uart, Read, size_t(std::span<uint8_t>)))
        .Exactly(kReturnByteFlag.size());
  }

  SECTION("Read() with timeout, returns smaller buffer size than expected")
  {
    // Setup
    int read_position                      = 0;
    static constexpr size_t kPayloadLength = 4;
    std::array<uint8_t, kPayloadLength> bytes;
    std::fill(bytes.begin(), bytes.end(), 0x00);

    When(OverloadedMethod(mock_uart, Read, size_t(std::span<uint8_t>)))
        .AlwaysDo([&read_position](std::span<uint8_t> data) -> size_t {
          std::array<uint8_t, kPayloadLength> response = {
            0xAA,
            0xBB,
            0xCC,
            0xDD,
          };

          // Returning bytes when read_position is below 3. When its above 3
          // only return 0 and allow Read() to timeout.
          if (read_position < 3)
          {
            data[0] = response[read_position++];
            return 1;
          }

          return 0;
        });

    // Exercise
    auto bytes_read = uart.Read(bytes, 10us);

    // Verify
    CHECK(kPayloadLength - 1 == bytes_read);
    CHECK(kPayloadLength - 1 == read_position);
    Verify(OverloadedMethod(mock_uart, Read, size_t(std::span<uint8_t>)))
        .AtLeast(kPayloadLength - 1);
    CHECK(0xAA == bytes[0]);
    CHECK(0xBB == bytes[1]);
    CHECK(0xCC == bytes[2]);
    CHECK(0x00 == bytes[3]);
  }
}
}  // namespace sjsu

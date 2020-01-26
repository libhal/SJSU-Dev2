#include <algorithm>
#include <array>

#include "L4_Testing/testing_frameworks.hpp"
#include "L1_Peripheral/uart.hpp"

namespace sjsu
{
TEST_CASE("Testing L1 uart", "[uart]")
{
  Mock<Uart> mock_uart;
  Fake(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t)));
  Fake(ConstOverloadedMethod(mock_uart, Read, size_t(void *, size_t)));
  Fake(Method(mock_uart, HasData));
  Uart & uart = mock_uart.get();

  SECTION("Write() Byte")
  {
    // Setup
    constexpr uint8_t kExpectedByte = 0x3F;
    uint8_t actual_byte             = 0;

    When(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t)))
        .Do([&actual_byte](const void * data, size_t size) -> void {
          CHECK(1 == size);
          actual_byte = *reinterpret_cast<const uint8_t *>(data);
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
    When(ConstOverloadedMethod(mock_uart, Write, void(const void *, size_t)))
        .Do([&write_was_called](const void * data_ptr, size_t size) -> void {
          const uint8_t * data = reinterpret_cast<const uint8_t *>(data_ptr);

          CHECK(5 == size);
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

    When(ConstOverloadedMethod(mock_uart, Read, size_t(void *, size_t)))
        .Do([kExpectedByte](void * data, size_t size) -> size_t {
          CHECK(1 == size);
          *reinterpret_cast<uint8_t *>(data) = kExpectedByte;
          return size;
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
    Verify(ConstOverloadedMethod(mock_uart, Read, size_t(void *, size_t)))
        .Exactly(3);
  }

  SECTION("Read() with timeout, returns kSuccess")
  {
    // Setup
    int read_was_called                    = 0;
    static constexpr size_t kPayloadLength = 4;
    std::array<uint8_t, kPayloadLength> bytes;
    std::fill(bytes.begin(), bytes.end(), 0x00);

    When(Method(mock_uart, HasData))
        .Return(true)   // Data already in buffer
        .Return(true)   // Data already in buffer
        .Return(true)   // Data already in buffer
        .Return(false)  // No data available, waiting
        .Return(false)  // No data available, waiting
        .Return(false)  // No data available, waiting
        .Return(true);  // More data has arrived;

    When(ConstOverloadedMethod(mock_uart, Read, size_t(void *, size_t)))
        .AlwaysDo([&read_was_called](void * data_ptr, size_t size) -> size_t {
          uint8_t * data = reinterpret_cast<uint8_t *>(data_ptr);
          std::array<uint8_t, kPayloadLength> response = {
            0xAA, 0xBB, 0xCC, 0xDD
          };
          for (int i = 0; i < size; i++)
          {
            data[i] = response[read_was_called++];
          }
          return size;
        });

    // Exercise
    Status status = uart.Read(bytes.data(), bytes.size(), 10us);

    // Verify
    CHECK(kPayloadLength == read_was_called);
    CHECK(Status::kSuccess == status);
    Verify(Method(mock_uart, HasData)).Exactly(7);
    Verify(ConstOverloadedMethod(mock_uart, Read, size_t(void *, size_t)))
        .Exactly(kPayloadLength);
    CHECK(0xAA == bytes[0]);
    CHECK(0xBB == bytes[1]);
    CHECK(0xCC == bytes[2]);
    CHECK(0xDD == bytes[3]);
  }

  SECTION("Read() with timeout, returns kTimeout")
  {
    // Setup
    int read_was_called                    = 0;
    static constexpr size_t kPayloadLength = 4;
    std::array<uint8_t, kPayloadLength> bytes;
    std::fill(bytes.begin(), bytes.end(), 0x00);

    When(Method(mock_uart, HasData))
        .Return(true)          // Data already in buffer
        .Return(true)          // Data already in buffer
        .Return(true)          // Data already in buffer
        .AlwaysReturn(false);  // No data available, waiting

    When(ConstOverloadedMethod(mock_uart, Read, size_t(void *, size_t)))
        .AlwaysDo([&read_was_called](void * data_ptr, size_t size) -> size_t {
          uint8_t * data = reinterpret_cast<uint8_t *>(data_ptr);
          std::array<uint8_t, kPayloadLength> response = {
            0xAA, 0xBB, 0xCC, 0xDD
          };
          for (int i = 0; i < size; i++)
          {
            data[i] = response[read_was_called++];
          }
          return size;
        });

    // Exercise
    Status status = uart.Read(bytes.data(), bytes.size(), 10us);

    // Verify
    CHECK(kPayloadLength - 1 == read_was_called);
    CHECK(Status::kTimedOut == status);
    Verify(Method(mock_uart, HasData)).Exactly(8);
    Verify(ConstOverloadedMethod(mock_uart, Read, size_t(void *, size_t)))
        .Exactly(kPayloadLength - 1);
    CHECK(0xAA == bytes[0]);
    CHECK(0xBB == bytes[1]);
    CHECK(0xCC == bytes[2]);
    CHECK(0x00 == bytes[3]);
  }
}
}  // namespace sjsu

#include "L4_Testing/testing_frameworks.hpp"
#include "L1_Peripheral/i2c.hpp"

namespace sjsu
{
TEST_CASE("Testing L1 i2c")
{
  // Dummy address used by test sections
  constexpr uint8_t kAddress = 0x33;

  Mock<I2c> mock_i2c;
  sjsu::I2c::Transaction_t actual_transaction;

  When(Method(mock_i2c, Transaction))
      .AlwaysDo([&actual_transaction](I2c::Transaction_t transaction) {
        actual_transaction = transaction;
      });

  I2c & test_subject = mock_i2c.get();

  SECTION("Transaction test")
  {
    constexpr uint32_t kFakeDeviceAddress = 0x22;
    sjsu::I2c::Transaction_t transaction;
    transaction.address = 0x22;

    transaction.operation = sjsu::I2c::Operation::kWrite;
    // With a read operation, the address is shifted by 1 and LSB is set to 0.
    CHECK((kFakeDeviceAddress << 1) == transaction.GetProperAddress());
    // With a read operation, the address is shifted by 1 and LSB is set to 1.
    transaction.operation = sjsu::I2c::Operation::kRead;
    CHECK(((kFakeDeviceAddress << 1) | 1) == transaction.GetProperAddress());
  }

  SECTION("Read Setup")
  {
    uint8_t read_buffer[10];

    test_subject.Read(kAddress, read_buffer, sizeof(read_buffer));

    CHECK(actual_transaction.address == kAddress);
    CHECK(actual_transaction.data_out == nullptr);
    CHECK(actual_transaction.out_length == 0);
    CHECK(actual_transaction.data_in == read_buffer);
    CHECK(actual_transaction.in_length == sizeof(read_buffer));
    CHECK(actual_transaction.position == 0);
    CHECK(actual_transaction.repeated == false);
    CHECK(actual_transaction.busy == true);
    CHECK(static_cast<int>(actual_transaction.status) == 0);
    CHECK(actual_transaction.operation == I2c::Operation::kRead);
    CHECK(actual_transaction.timeout == I2c::kI2cTimeout);
  }

  SECTION("Write Setup")
  {
    uint8_t write_buffer[10];

    test_subject.Write(kAddress, write_buffer, sizeof(write_buffer));

    CHECK(actual_transaction.address == kAddress);
    CHECK(actual_transaction.data_out == write_buffer);
    CHECK(actual_transaction.out_length == sizeof(write_buffer));
    CHECK(actual_transaction.data_in == nullptr);
    CHECK(actual_transaction.in_length == 0);
    CHECK(actual_transaction.position == 0);
    CHECK(actual_transaction.repeated == false);
    CHECK(actual_transaction.busy == true);
    CHECK(static_cast<int>(actual_transaction.status) == 0);
    CHECK(actual_transaction.operation == I2c::Operation::kWrite);
    CHECK(actual_transaction.timeout == I2c::kI2cTimeout);
  }

  SECTION("Write and Read Setup")
  {
    uint8_t read_buffer[10];
    uint8_t write_buffer[15];

    test_subject.WriteThenRead(kAddress, write_buffer, sizeof(write_buffer),
                               read_buffer, sizeof(read_buffer));

    CHECK(actual_transaction.address == kAddress);
    CHECK(actual_transaction.data_out == write_buffer);
    CHECK(actual_transaction.out_length == sizeof(write_buffer));
    CHECK(actual_transaction.data_in == read_buffer);
    CHECK(actual_transaction.in_length == sizeof(read_buffer));
    CHECK(actual_transaction.position == 0);
    CHECK(actual_transaction.repeated == true);
    CHECK(actual_transaction.busy == true);
    CHECK(static_cast<int>(actual_transaction.status) == 0);
    CHECK(actual_transaction.operation == I2c::Operation::kWrite);
    CHECK(actual_transaction.timeout == I2c::kI2cTimeout);
  }
}
}  // namespace sjsu

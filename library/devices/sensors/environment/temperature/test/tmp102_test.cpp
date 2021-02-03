#include "devices/sensors/environment/temperature/tmp102.hpp"

#include "testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing Tmp102 Temperature Sensor")
{
  constexpr uint8_t kDeviceAddress = Tmp102::DeviceAddress::kGround;
  Mock<I2c> mock_i2c;
  Tmp102 temperature_sensor(mock_i2c.get(), kDeviceAddress);

  SECTION("Initialize")
  {
    // Setup
    Fake(Method(mock_i2c, ModuleInitialize));

    // Exercise
    temperature_sensor.ModuleInitialize();

    // Verify
    Verify(Method(mock_i2c, ModuleInitialize)).Once();
  }

  SECTION("GetTemperature")
  {
    // GetTemperature() requires two I2C transactions:
    // 1. Write the shutdown mode command to the configuration register.
    // 2. Write the temperature register address and read the temperature value.

    // 12-bit temperature data to inject stored in bits [15:3].
    const std::array kExpectedTemperatureData = { 0x7F, 0xF0 };
    // From Table 2 in the datasheet, the temperature in degrees celsius for
    // 0x7FF is 127.9375˚C.
    constexpr float kExpectedTemperature         = 127.9375f;
    constexpr float kMarginOfError               = 0.001f;
    constexpr std::array kExpectedDataOutLengths = { 2, 1 };
    constexpr uint8_t kMaxDataLength = kExpectedDataOutLengths.size();
    const uint8_t kExpectedDataOut[][kMaxDataLength] = {
      { Tmp102::RegisterAddress::kConfiguration, Tmp102::kOneShotShutdownMode },
      { Tmp102::RegisterAddress::kTemperature },
    };

    const std::array kExpectedTransactions = {
      I2c::Transaction_t({
          .operation  = I2c::Operation::kWrite,
          .address    = kDeviceAddress,
          .data_out   = kExpectedDataOut[0],
          .out_length = kExpectedDataOutLengths[0],
          .data_in    = nullptr,
          .in_length  = 0,
          .position   = 0,
          .repeated   = false,
          .busy       = true,
          .timeout    = I2c::kI2cTimeout,
      }),
      I2c::Transaction_t({
          .operation  = I2c::Operation::kWrite,
          .address    = kDeviceAddress,
          .data_out   = kExpectedDataOut[1],
          .out_length = kExpectedDataOutLengths[1],
          .data_in    = nullptr,
          .in_length  = std::size(kExpectedTemperatureData),
          .position   = 0,
          .repeated   = true,
          .busy       = true,
          .timeout    = Tmp102::kConversionTimeout,
      }),
    };

    // Array to store the two transactions.
    I2c::Transaction_t transactions[kExpectedTransactions.size()];
    // Storing Transaction's data_out in separate array since it's read-only.
    uint8_t data_out[kExpectedTransactions.size()][kMaxDataLength];
    // Stub Transaction to extract each Transaction record and to inject data to
    // receive buffer when temperature data is expected to be received.
    When(Method(mock_i2c, Transaction))
        .AlwaysDo([&transactions, &data_out, &kExpectedTemperatureData](
                      I2c::Transaction_t transaction) -> void {
          static uint8_t transaction_id = 0;
          transactions[transaction_id]  = transaction;

          for (size_t i = 0; i < transaction.out_length; i++)
          {
            data_out[transaction_id][i] = transaction.data_out[i];
          }

          // Inject temperature data to second transaction which is the
          // transaction to fetch temperature data.
          if (transaction_id == 1)
          {
            transaction.data_in[0] = kExpectedTemperatureData[0];
            transaction.data_in[1] = kExpectedTemperatureData[1];
          }

          transaction_id++;
        });

    auto temperature = temperature_sensor.GetTemperature();

    // Verify the configuration and data in the two transactions.
    Verify(Method(mock_i2c, Transaction)).Exactly(kExpectedTransactions.size());
    for (size_t i = 0; i < kExpectedTransactions.size(); i++)
    {
      INFO("Checking transaction: #" << (i + 1));
      CHECK(transactions[i].operation == kExpectedTransactions[i].operation);
      CHECK(transactions[i].address == kExpectedTransactions[i].address);
      CHECK(transactions[i].out_length == kExpectedTransactions[i].out_length);
      CHECK(transactions[i].in_length == kExpectedTransactions[i].in_length);
      CHECK(transactions[i].position == kExpectedTransactions[i].position);
      CHECK(transactions[i].repeated == kExpectedTransactions[i].repeated);
      CHECK(transactions[i].timeout == kExpectedTransactions[i].timeout);
      CHECK(transactions[i].status == kExpectedTransactions[i].status);
      for (size_t j = 0; j < transactions[i].out_length; j++)
      {
        CHECK(data_out[i][j] == kExpectedTransactions[i].data_out[j]);
      }
    }
    REQUIRE(temperature);
    CHECK(temperature.to<float>() ==
          doctest::Approx(kExpectedTemperature).epsilon(kMarginOfError));
  }
}
}  // namespace sjsu

#include <inttypes.h>
#include <cstdint>
#include "L0_LowLevel/interrupt.hpp"
#include "L1_Drivers/i2c.hpp"
#include "utility/log.hpp"

constexpr uint8_t kFirstI2cAddress = 0x08;
constexpr uint8_t kLastI2cAddress  = 0x78;

constexpr uint8_t kAccelerometerAddress = 0x1C;
uint8_t byte                            = 0x0D;

I2c i2c;

int main(void)
{
  LOG_INFO("I2C Application Starting...");
  LOG_INFO(
      "This example will scan I2C Bus 2 for any devices. If the transaction "
      "comes back with Status::kSuccess, then we know that an I2C device has "
      "acknowledged our call and it exists.");

  LOG_INFO("Initializing I2C Port 2...");
  i2c.Initialize();
  LOG_INFO("Initializing Onboard Accelerometer using I2C.2...");
  // Accelerometer initialization sequence of setting register 0x2A, Control
  // register 1, to value 0x01. This sets the first bit, ACTIVE, to enabled and
  // clears the rest.
  i2c.Write(kAccelerometerAddress, { 0x2A, 0x01 });

  while (true)
  {
    LOG_INFO("Starting Scan...");
    Status status;
    for (uint8_t address = kFirstI2cAddress; address < kLastI2cAddress;
         address++)
    {
      status = i2c.Write(address, nullptr, 0, 50);
      if (status == Status::kSuccess)
      {
        LOG_INFO("    Found device at address: 0x%02X", address);
      }
    }

    status = i2c.WriteThenRead(kAccelerometerAddress, &byte, 1, &byte, 1);
    LOG_INFO("I2C transaction Status = 0x%02X",
                static_cast<uint8_t>(status));
    LOG_INFO("Accelerometer ID = 0x%02X", byte);

    LOG_INFO("Waiting 5s before starting the scan again...");
    Delay(5000);
  }
  return 0;
}

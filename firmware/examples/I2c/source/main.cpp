#include <inttypes.h>
#include <cstdint>
#include "L0_LowLevel/startup.hpp"
#include "L1_Drivers/i2c.hpp"
#include "L2_Utilities/debug_print.hpp"

constexpr uint8_t kFirstI2cAddress = 0x08;
constexpr uint8_t kLastI2cAddress  = 0x78;

constexpr uint8_t kAccelerometerAddress = 0x1C;
uint8_t initialization_sequence[]       = { 0x2A, 0x01 };
uint8_t byte                            = 0x0D;

I2c i2c;

int main(void)
{
  DEBUG_PRINT("I2C Application Starting...");
  DEBUG_PRINT(
      "This example will scan I2C Bus 2 for any devices. If the transaction "
      "comes back with I2cInterface::Status::kSuccess, then we know that an "
      "I2C device has acknowledged our call and it exists.");

  DEBUG_PRINT("Initializing I2C Port 2...");
  i2c.Initialize();
  DEBUG_PRINT("Initializing Onboard Accelerometer using I2C.2...");
  i2c.Write(kAccelerometerAddress, initialization_sequence,
            sizeof(initialization_sequence));

  while (true)
  {
    DEBUG_PRINT("Starting Scan...");
    I2cInterface::Status status;
    for (uint8_t address = kFirstI2cAddress; address < kLastI2cAddress;
         address++)
    {
      status = i2c.Write(address, nullptr, 0, 50);
      if (status == I2cInterface::Status::kSuccess)
      {
        DEBUG_PRINT("    Found device at address: 0x%02X", address);
      }
    }

    status = i2c.WriteThenRead(kAccelerometerAddress, &byte, 1, &byte, 1);
    DEBUG_PRINT("I2C transaction Status = 0x%02X",
                static_cast<uint8_t>(status));
    DEBUG_PRINT("Accelerometer ID = 0x%02X", byte);

    DEBUG_PRINT("Waiting 5s before starting the scan again...");
    Delay(5000);
  }
  return 0;
}

#include <cstdint>

#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "utility/log.hpp"

constexpr uint8_t kFirstI2cAddress      = 0x08;
constexpr uint8_t kLastI2cAddress       = 0x78;
constexpr uint8_t kAccelerometerAddress = 0x1C;
const uint8_t kAccelerometerIdRegister  = 0x0D;

int main()
{
  sjsu::LogInfo("I2C Application Starting...");
  sjsu::LogInfo(
      "This example will scan I2C Bus 2 for any devices. If the transaction "
      "comes back with Status::kSuccess, then we know that an I2C device has "
      "acknowledged our call and it exists.");

  sjsu::LogInfo("Initializing I2C Port 2...");
  sjsu::lpc40xx::I2c i2c(sjsu::lpc40xx::I2c::Bus::kI2c2);
  i2c.Initialize();
  sjsu::LogInfo("Initializing Onboard Accelerometer using I2C.2...");
  // Accelerometer initialization sequence of setting register 0x2A, Control
  // register 1, to value 0x01. This sets the first bit, ACTIVE, to enabled and
  // clears the rest.
  i2c.Write(kAccelerometerAddress, { 0x2A, 0x01 });

  while (true)
  {
    sjsu::LogInfo("Starting Scan...");
    sjsu::Status status;
    for (uint8_t address = kFirstI2cAddress; address < kLastI2cAddress;
         address++)
    {
      status = i2c.Write(address, nullptr, 0, 50ms);
      if (status == sjsu::Status::kSuccess)
      {
        sjsu::LogInfo("    Found device at address: 0x%02X", address);
      }
    }

    uint8_t id = 0;

    status = i2c.WriteThenRead(
        kAccelerometerAddress, &kAccelerometerIdRegister, 1, &id, 1);
    sjsu::LogInfo("I2C transaction Status = 0x%02X",
                  static_cast<uint8_t>(status));
    sjsu::LogInfo("Accelerometer ID = 0x%02X", id);

    sjsu::LogInfo("Waiting 5s before starting the scan again...");
    sjsu::Delay(5000ms);
  }
  return 0;
}

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
      "comes back without an error, then we know that an I2C device has "
      "acknowledged our call and it exists.");

  sjsu::LogInfo("Initializing I2C Bus 2...");
  sjsu::lpc40xx::I2c i2c(sjsu::lpc40xx::I2c::Bus::kI2c2);
  i2c.Initialize();

  while (true)
  {
    sjsu::LogInfo("Starting Scan...");
    for (uint8_t address = kFirstI2cAddress; address < kLastI2cAddress;
         address++)
    {
      uint8_t buffer;
      try
      {
        i2c.Read(address, &buffer, sizeof(buffer), 50ms);
        sjsu::LogInfo("    Found device at address: 0x%02X", address);
      }
      catch (sjsu::Exception & e)
      {
        // Skip exception handling as this is expected from this procedure
      }
    }

    sjsu::LogInfo("Waiting 5s before starting the scan again...");
    sjsu::Delay(5000ms);
  }
  return 0;
}

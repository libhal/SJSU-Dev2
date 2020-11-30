#include <cstdint>

#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "utility/log.hpp"

constexpr uint8_t kFirstI2cAddress      = 0x08;
constexpr uint8_t kLastI2cAddress       = 0x78;
constexpr uint8_t kAccelerometerAddress = 0x1C;

int main()
{
  sjsu::LogInfo("I2C Application Starting...");
  sjsu::LogInfo("This example will scan for devices on the I2C Bus 2.");

  sjsu::lpc40xx::I2c i2c(sjsu::lpc40xx::I2c::Bus::kI2c2);

  // Initialize I2C hardware
  i2c.Initialize();

  // Set the default 100kHz clock rate.
  i2c.ConfigureClockRate();

  // Enable i2c hardware
  i2c.Enable();

  sjsu::LogInfo("Starting Scan...");

  for (uint8_t address = kFirstI2cAddress; address < kLastI2cAddress; address++)
  {
    std::array<uint8_t, 1> buffer;
    try
    {
      i2c.Read(address, buffer, 50ms);
      sjsu::LogInfo("    Found device at address: 0x%02X", address);
    }
    catch (sjsu::Exception & e)
    {
      // We are expecting this error code from i2c operations tha fail to find
      // a device, but if the error is not the expected 1, re-throw the error
      // up.
      if (e.GetCode() != std::errc::no_such_device_or_address)
      {
        throw;
      }
    }
  }

  sjsu::LogInfo("Reset device to repeat demo...");
  return 0;
}

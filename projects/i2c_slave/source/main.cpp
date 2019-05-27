#include <cstdint>
#include "i2c_slave.hpp"
#include "L0_Platform/lpc40xx/interrupt.hpp"
#include "utility/log.hpp"
// Slave Board sample code reference
int main(void)
{
  LOG_INFO("I2C Slave Application Starting!!");
  // Create I2c object (defaults to I2C2)
  I2c i2c;
  // Pick any address other than an existing ones on the board. Use `i2c
  // discover` to see what those are.
  const uint8_t kSlaveAddr = 0xC0;
  // Our devices read/write buffer (This is the memory your master board will
  // read/write)
  volatile uint8_t buffer[256] = { 0 };
  // I2C is already initialized before main(), so you will have to add
  // initSlave() to i2c base class for your slave driver
  i2c.InitializeSlave(/* kSlaveAddr, buffer, sizeof(buffer)*/);

  // I2C interrupt will (should) modify our buffer.
  // So monitor the buffer, and print and/or light up LEDs
  // ie: If buffer[0] == 0, then LED ON, else LED OFF
  uint8_t prev = buffer[0];
  while (true)
  {
    if (prev != buffer[0])
    {
      printf(
          "buffer[0] changed from 0x%08X to 0x%08X by the other Master Board\n",
          prev, buffer[0]);
      prev = buffer[0];
    }
  }
  return 0;
}

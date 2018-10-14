#include <inttypes.h>
#include <cstdint>
#include "L0_LowLevel/startup.hpp"
#include "L1_Drivers/i2c.hpp"
#include "L2_Utilities/debug_print.hpp"
#include "L3_HAL/device_memory_map.hpp"

constexpr uint8_t kAccelerometerAddress = 0x1C;
constexpr uint8_t kGestureAddress       = 0x39;

template <WriteFnt write, ReadFnt read>
SJ2_PACKED(struct)
AccelerometerMemoryMap_t  // MMA8452Q
{
  template <typename Int>
  using Register = device::Register_t<Int, write, read>;

  Register<uint8_t> status;
  Register<uint16_t> x;
  Register<uint16_t> y;
  Register<uint16_t> z;
  device::Reserved_t<0x04> reserved0;
  Register<uint8_t> sysmod;
  Register<uint8_t> int_source;
  Register<uint8_t> who_am_i;
  device::Reserved_t<0x1C> reserved1;
  Register<uint8_t> control1;
  Register<uint8_t> control2;
  Register<uint8_t> control3;
};

template <WriteFnt write, ReadFnt read>
SJ2_PACKED(struct)
GestureMemoryMap_t  // APDS-9960
{
  template <typename Int>
  using Register = device::Register_t<Int, write, read>;

  device::Reserved_t<0x80> ram;
  Register<uint8_t> enable;
  Register<uint8_t> adc_integration_time;
};

I2c i2c;

I2cDevice<&i2c, kAccelerometerAddress, device::Endian::kBig,
          AccelerometerMemoryMap_t>
    accelerometer;

I2cDevice<&i2c, kGestureAddress, device::Endian::kLittle, GestureMemoryMap_t>
    gesture;

int main(void)
{
  DEBUG_PRINT("I2C MemoryMapped Device Starting...");
  DEBUG_PRINT("Initializing I2C Port 2...");
  i2c.Initialize();
  DEBUG_PRINT("Initializing Onboard Accelerometer using I2C.2...");
  accelerometer.memory.control1       = 1;
  gesture.memory.adc_integration_time = 0;

  while (true)
  {
    uint8_t status = accelerometer.memory.status;
    DEBUG_PRINT("Status 0x%02X", status);

    uint8_t who_am_i = accelerometer.memory.who_am_i;
    DEBUG_PRINT("WHO_AM_I 0x%02X", who_am_i);

    uint16_t x = accelerometer.memory.x;
    uint16_t y = accelerometer.memory.y;
    uint16_t z = accelerometer.memory.z;
    DEBUG_PRINT("x = %d :: y = %d :: z = %d", x, y, z);

    uint8_t reg1;
    reg1 = gesture.memory.adc_integration_time;
    DEBUG_PRINT("cur reg1 = 0x%02X", reg1);

    gesture.memory.adc_integration_time = 5;
    reg1                                = gesture.memory.adc_integration_time;
    DEBUG_PRINT("new reg1 = 0x%02X", reg1);

    gesture.memory.adc_integration_time |= (1 << 7);
    reg1 = gesture.memory.adc_integration_time;
    DEBUG_PRINT("ord reg1 = 0x%02X", reg1);

    gesture.memory.adc_integration_time = 255;
    reg1                                = gesture.memory.adc_integration_time;
    DEBUG_PRINT("add reg1 = 0x%02X", reg1);
    DEBUG_PRINT("========================\n");

    Delay(2000);
  }
  return 0;
}

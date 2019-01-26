#include <inttypes.h>
#include <cstdint>
#include "L0_LowLevel/interrupt.hpp"
#include "L1_Drivers/i2c.hpp"
#include "L2_HAL/device_memory_map.hpp"
#include "utility/log.hpp"

constexpr uint8_t kAccelerometerAddress = 0x1C;
constexpr uint8_t kGestureAddress       = 0x39;

template <device::Endian endianess, WriteFnt write, ReadFnt read>
SJ2_PACKED(struct)
AccelerometerMemoryMap_t  // MMA8452Q
{
  template <typename Int>
  using Register = device::Register_t<Int, endianess, write, read>;

  template <typename Int, size_t kLength>
  using Array = device::Array_t<Int, kLength, write, read>;

  Register<uint8_t> status;
  union
  {
    Array<uint8_t, 6> xyz;
    struct
    {
      Register<uint16_t> x;
      Register<uint16_t> y;
      Register<uint16_t> z;
    };
  };
  device::Reserved_t<0x04> reserved0;
  Register<uint8_t> sysmod;
  Register<uint8_t> int_source;
  Register<uint8_t> who_am_i;
  device::Reserved_t<0x1C> reserved1;
  union
  {
    Array<uint8_t, 3> control;
    struct
    {
      Register<uint8_t> control1;
      Register<uint8_t> control2;
      Register<uint8_t> control3;
    };
  };
};

template <device::Endian endianess, WriteFnt write, ReadFnt read>
SJ2_PACKED(struct)
GestureMemoryMap_t  // APDS-9960
{
  template <typename Int>
  using Register = device::Register_t<Int, endianess, write, read>;

  template <typename Int, size_t kLength>
  using Array = device::Array_t<Int, kLength, write, read>;

  device::Reserved_t<0x80> ram;
  Register<uint8_t> enable;
  Register<uint8_t> adc_integration_time;
};

I2c i2c;

I2cDevice<&i2c, kAccelerometerAddress, device::Endian::kBig,
          AccelerometerMemoryMap_t> accelerometer;

I2cDevice<&i2c, kGestureAddress, device::Endian::kLittle, GestureMemoryMap_t>
    gesture;

int main(void)
{
  LOG_INFO("I2C MemoryMapped Device Starting...");
  LOG_INFO("Initializing I2C Port 2...");
  i2c.Initialize();
  LOG_INFO("Initializing Onboard Accelerometer using I2C.2...");
  accelerometer.memory.control[0]     = (1 << 0);
  gesture.memory.adc_integration_time = 0;

  while (true)
  {
    uint8_t status = accelerometer.memory.status;
    LOG_INFO("Status 0x%02X", status);

    uint8_t who_am_i = accelerometer.memory.who_am_i;
    LOG_INFO("WHO_AM_I 0x%02X", who_am_i);

    uint16_t x = accelerometer.memory.x;
    uint16_t y = accelerometer.memory.y;
    uint16_t z = accelerometer.memory.z;
    LOG_INFO("x = %d :: y = %d :: z = %d", x, y, z);

    [[maybe_unused]] std::array<uint8_t, 6> xyz = accelerometer.memory.xyz;
    uint8_t x_msb = accelerometer.memory.xyz[0];
    uint8_t x_lsb = accelerometer.memory.xyz[1];
    LOG_INFO("x = 0x%02X%02X", x_msb, x_lsb);

    uint8_t reg1;
    reg1 = gesture.memory.adc_integration_time;
    LOG_INFO("cur reg1 = 0x%02X", reg1);

    gesture.memory.adc_integration_time = 5;
    reg1 = gesture.memory.adc_integration_time;
    LOG_INFO("new reg1 = 0x%02X", reg1);

    gesture.memory.adc_integration_time |= (1 << 7);
    reg1 = gesture.memory.adc_integration_time;
    LOG_INFO("ord reg1 = 0x%02X", reg1);

    gesture.memory.adc_integration_time = 255;
    reg1 = gesture.memory.adc_integration_time;
    LOG_INFO("add reg1 = 0x%02X", reg1);
    LOG_INFO("========================\n");

    Delay(500);
  }
  return 0;
}

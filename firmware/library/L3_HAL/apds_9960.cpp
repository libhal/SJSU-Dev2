#include "L1_Drivers/i2c.hpp"
#include "L3_HAL/apds_9960.hpp"
#include "L3_HAL/device_memory_map.hpp"

I2c Apds9960::i2c;
I2cDevice<&Apds9960::i2c, 0x39,
  device::Endian::kLittle, Apds9960Interface::MemoryMap_t> Apds9960::gesture;

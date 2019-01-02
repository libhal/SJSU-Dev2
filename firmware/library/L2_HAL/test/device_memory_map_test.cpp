#include "L2_HAL/device_memory_map.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace {

template <device::Endian endianess, WriteFnt write, ReadFnt read>
SJ2_PACKED(struct)
TestMemoryMap_t
{
  template <typename Int>
  using Register = device::Register_t<Int, endianess, write, read>;

  template <typename Int, size_t kLength>
  using Array = device::Array_t<Int, kLength, write, read>;

  Register<uint8_t> register0;
  union
  {
    Array<uint8_t, 6> union_register;
    struct
    {
      Register<uint16_t> union_0;
      Register<uint16_t> union_1;
      Register<uint16_t> union_2;
    };
  };
  device::Reserved_t<0x04> reserved0;
  Register<uint32_t> register1;
  Register<uint16_t> register2;
  Register<uint8_t> register3;
  Register<uint8_t> register4;
  Array<uint32_t, 4> array_0;
};

I2c test_i2c;

}  // namespace

TEST_CASE("Testing Device Memory Map", "[device_memory_map]")
{
  I2cDevice<&test_i2c, 0x39, device::Endian::kLittle, TestMemoryMap_t> test;
  SECTION("Initialize")
  {
  }
}

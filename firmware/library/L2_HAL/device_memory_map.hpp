#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <type_traits>

#include "L1_Drivers/i2c.hpp"
#include "utility/macros.hpp"
#include "utility/status.hpp"

using WriteFnt = bool (*)(intptr_t, size_t, uint8_t *);
using ReadFnt  = void (*)(intptr_t, size_t, uint8_t *);

#define MEMORY_OPERATION(op)                                \
  bool operator op##=(Int value)                            \
  {                                                         \
    Int register_value = this->operator Int();              \
    bool status = this->operator=(register_value op value); \
    return status;                                          \
  }

namespace device
{
enum class Endian
{
  kLittle,
  kBig
};

template <typename Int, Endian endianess, WriteFnt write, ReadFnt read>
SJ2_PACKED(struct)
Register_t
{
  static uint32_t BigEndianessSwap(size_t size, uint32_t value)
  {
    value = __builtin_bswap32(value);
    value = value >> ((sizeof(value) - size) * 8);
    return value;
  }
  bool operator=(Int value)
  {
    intptr_t address = reinterpret_cast<intptr_t>(this);
    Int local        = value;
    if (endianess == Endian::kBig)
    {
      local = static_cast<Int>(BigEndianessSwap(sizeof(Int), local));
    }
    bool status =
        write(address, sizeof(Int), reinterpret_cast<uint8_t *>(&local));
    return status;
  }
  MEMORY_OPERATION(|);
  MEMORY_OPERATION(&);
  MEMORY_OPERATION(^);
  MEMORY_OPERATION(+);
  MEMORY_OPERATION(-);
  MEMORY_OPERATION(*);
  MEMORY_OPERATION(/);
  MEMORY_OPERATION(>>);
  MEMORY_OPERATION(<<);
  // The purpose of the NOLINT comment is due to the fact clang-tidy is set
  // to force this operator to be explicit. But that allowing this object to
  // be implicitly converted allows it to work when assigned to integer types
  operator Int()  // NOLINT
  {
    intptr_t address = reinterpret_cast<intptr_t>(this);
    Int result       = 0;
    read(address, sizeof(Int), reinterpret_cast<uint8_t *>(&result));
    if (endianess == Endian::kBig)
    {
      result = static_cast<Int>(BigEndianessSwap(sizeof(Int), result));
    }
    return result;
  }
  Int member_;
};

#define RESERVED_OPERATION(op)                                          \
  template <typename T>                                                 \
  constexpr bool operator op##=(T)                                      \
  {                                                                     \
    static_assert(CallCheck<T>::kValue,                                 \
                  "Reserved memory cannot be assigned or manipulated"); \
    return false;                                                       \
  }

template <size_t kLength>
SJ2_PACKED(struct)
Reserved_t
{
  template <typename T>
  struct CallCheck
  {
    static constexpr bool kValue = false;
  };

  template <typename T>
  constexpr bool operator=(T)
  {
    // This compile time check only happens when this variable is assigned.
    // Originally, false was in placed of the CallCheck structure. But this
    // would fire off regardless of the usage of operator=. So the structure
    // is used to prevent the compiler from complaining before an assignment
    // is made.
    static_assert(CallCheck<T>::kValue,
                  "Reserved memory cannot be assigned or manipulated");
    return false;
  }
  RESERVED_OPERATION(|);
  RESERVED_OPERATION(&);
  RESERVED_OPERATION(^);
  RESERVED_OPERATION(+);
  RESERVED_OPERATION(-);
  RESERVED_OPERATION(*);
  RESERVED_OPERATION(/);
  RESERVED_OPERATION(>>);
  RESERVED_OPERATION(<<);
  // The purpose of the NOLINT comment is due to the fact clang-tidy is set
  // to force this operator to be explicit. But that allowing this object to
  // be implicitly converted allows it to work when assigned to integer types
  template <typename T>
  constexpr operator T()  // NOLINT
  {
    // Same explanation as "operator="
    static_assert(CallCheck<T>::kValue, "Reserved memory cannot be retrieved");
    return 0;
  }
  uint8_t padding_[kLength];
};

template <typename Int, size_t kLength, WriteFnt write, ReadFnt read>
SJ2_PACKED(struct)
Array_t
{
  template <size_t kSize>
  bool operator=(std::array<Int, kSize> data)
  {
    intptr_t address = reinterpret_cast<intptr_t>(this);
    size_t length    = std::min(kSize, kLength);
    bool status      = write(address, length, data.data());
    return status;
  }
  Register_t<Int, Endian::kLittle, write, read> & operator[](size_t index)
  {
    return member_[index];
  }
  // The purpose of the NOLINT comment is due to the fact clang-tidy is set
  // to force this operator to be explicit. But that allowing this object to
  // be implicitly converted allows it to work when assigned to integer types
  template <size_t kSize>
  operator std::array<Int, kSize>()  // NOLINT
  {
    intptr_t address = reinterpret_cast<intptr_t>(this);
    std::array<Int, kSize> result;
    size_t length = std::min(kSize, kLength);
    read(address, length, result.data());
    return result;
  }
  std::array<Register_t<Int, Endian::kLittle, write, read>, kLength> member_;
};

}  // namespace device

template <class DeviceProtocol, device::Endian endianess,
          template <device::Endian endian, WriteFnt write, ReadFnt read>
          class MemoryMap>
class Device
{
 private:
  using success = uint8_t;
  using failure = uint16_t;

  template <typename ClassUnderTest>
  static success TestWriteExists(decltype(ClassUnderTest::Write));
  template <typename ClassUnderTest>
  static failure TestWriteExists(...);
  template <typename ClassUnderTest>
  static success TestReadExists(decltype(ClassUnderTest::Read));
  template <typename ClassUnderTest>
  static failure TestReadExists(...);

  template <typename ProtocolImplementor>
  static bool Write(intptr_t address, size_t size, uint8_t * value)
  {
    return ProtocolImplementor::Write(address, size, value);
  }
  template <typename ProtocolImplementor>
  static void Read(intptr_t address, size_t size, uint8_t * value)
  {
    return ProtocolImplementor::Read(address, size, value);
  }

 public:
  static constexpr bool kWriteExists =
      (sizeof(TestWriteExists<DeviceProtocol>(0)) == sizeof(success));
  static constexpr bool kReadExists =
      (sizeof(TestReadExists<DeviceProtocol>(0)) == sizeof(success));

  MemoryMap<endianess, Device::Write<DeviceProtocol>,
            Device::Read<DeviceProtocol>> & memory;
  using MapType = std::remove_reference_t<decltype(memory)>;
  static constexpr MapType * kMapAtAddressZero = static_cast<MapType *>(0);
  // The I2cDevice constructor's only job in this case is to set the memory
  // ptr to 0. This will align the memory map to address zero. So when the
  // registers lookup their own address location, what they are really getting
  // is their position in the memory map.
  constexpr Device() : memory(*kMapAtAddressZero)
  {
    static_assert(kWriteExists,
                  "Device Memory Map implementations need to implement a "
                  "static 'bool Write(intptr_t, size_t, uint32_t)' to "
                  "write over the hardware protocol.");
    static_assert(kReadExists,
                  "Device Memory Map implementations need to implement a "
                  "'static uint32_t Read(intptr_t, size_t, uint32_t)' to "
                  "read over the hardware protocol.");
  }
};

template <I2c * i2c, const uint8_t kDeviceAddress, device::Endian endianess,
          template <device::Endian endian, WriteFnt write, ReadFnt read>
          class MemoryMap>
class I2cDevice
    : public Device<I2cDevice<i2c, kDeviceAddress, endianess, MemoryMap>,
                    endianess, MemoryMap>
{
 public:
  // Standard Write transaction for most I2C devices
  static bool Write(intptr_t address, size_t size, uint8_t * target)
  {
    constexpr size_t kMaxPayloadLength = 128;
    uint8_t payload[kMaxPayloadLength];
    payload[0] = static_cast<uint8_t>(address);
    memcpy(&payload[1], target, size);
    // Size + 1 to account for the 1-byte register address
    Status status = i2c->Write(kDeviceAddress, payload, size + 1);
    return (status == Status::kSuccess);
  }
  // Standard Read transaction for most I2C devices
  static void Read(intptr_t address, size_t size, uint8_t * target)
  {
    uint8_t register_address = static_cast<uint8_t>(address);
    i2c->WriteThenRead(kDeviceAddress, &register_address, 1, target, size);
  }
  I2cDevice() {}
};

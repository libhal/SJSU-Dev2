#pragma once
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <type_traits>
#include "L0_LowLevel/delay.hpp"
#include "L0_LowLevel/interrupt.hpp"
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/i2c.hpp"
#include "L2_Utilities/enum.hpp"
#include "L2_Utilities/macros.hpp"

using WriteFnt = bool (*)(intptr_t, size_t, uint32_t);
using ReadFnt  = uint32_t (*)(intptr_t, size_t);

template <class DeviceProtocol,
          template <WriteFnt write, ReadFnt read> class MemoryMap>
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
  static bool Write(intptr_t address, size_t size, uint32_t value)
  {
    return ProtocolImplementor::Write(address, size, value);
  }
  template <typename ProtocolImplementor>
  static uint32_t Read(intptr_t address, size_t size)
  {
    return ProtocolImplementor::Read(address, size);
  }

 public:
  static constexpr bool kWriteExists =
      (sizeof(TestWriteExists<DeviceProtocol>(0)) == sizeof(success));
  static constexpr bool kReadExists =
      (sizeof(TestReadExists<DeviceProtocol>(0)) == sizeof(success));

  MemoryMap<Device::Write<DeviceProtocol>, Device::Read<DeviceProtocol>> &
      memory;
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

template <typename Int, WriteFnt write, ReadFnt read>
SJ2_PACKED(struct)
Register_t
{
  bool operator=(Int value)
  {
    intptr_t address = reinterpret_cast<intptr_t>(this);
    bool status      = write(address, sizeof(member_), value);
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
    return static_cast<Int>(read(address, sizeof(member_)));
  }
  Int member_;
};

#define RESERVED_OPERATION(op)                                          \
  template <typename T>                                                 \
  constexpr bool operator op##=(T value)                                \
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
  constexpr bool operator=(T value)
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
}  // namespace device

template <I2c * i2c, const uint8_t kDeviceAddress, device::Endian endianess,
          template <WriteFnt write, ReadFnt read> class MemoryMap>
class I2cDevice
    : public Device<I2cDevice<i2c, kDeviceAddress, endianess, MemoryMap>,
                    MemoryMap>
{
 public:
  SJ2_PACKED(union) WritePayload_t
  {
    SJ2_PACKED(struct) Payload_t
    {
      uint8_t address;
      uint32_t value;
    };
    Payload_t payload;
    uint8_t data[sizeof(payload)];
  };

  SJ2_PACKED(union) ReadPayload_t
  {
    uint32_t value;
    uint8_t data[sizeof(value)];
  };

  static uint32_t BigEndianessSwap(size_t size, uint32_t value)
  {
    value = __builtin_bswap32(value);
    value = value >> ((sizeof(value) - size) * 8);
    return value;
  }
  // Standard Write transaction for most I2C devices
  static bool Write(intptr_t address, size_t size, uint32_t value)
  {
    WritePayload_t val;

    using ValueType   = decltype(val.payload.value);
    using AddressType = decltype(val.payload.address);
    // Assumption that core architecture is little endian to begin with
    if (endianess == device::Endian::kBig)
    {
      value = BigEndianessSwap(size, value);
    }
    val.payload.value   = static_cast<ValueType>(value);
    val.payload.address = static_cast<AddressType>(address);
    // Size + 1 to account for the 1-byte register address
    I2cInterface::Status status =
        i2c->Write(kDeviceAddress, val.data, size + 1);
    return (status == I2cInterface::Status::kSuccess);
  }
  // Standard Read transaction for most I2C devices
  static uint32_t Read(intptr_t address, size_t size)
  {
    ReadPayload_t read;
    uint8_t register_address = static_cast<uint8_t>(address);
    i2c->WriteThenRead(kDeviceAddress, &register_address, 1, read.data, size);
    uint32_t result = 0;
    if (endianess == device::Endian::kBig)
    {
      result = BigEndianessSwap(size, read.value);
    }
    else
    {
      result = read.value;
    }
    return result;
  }
  I2cDevice() {}
};

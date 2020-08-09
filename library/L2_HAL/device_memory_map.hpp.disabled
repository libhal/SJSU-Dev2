#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>

#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "utility/macros.hpp"
#include "utility/status.hpp"
#include "utility/bit.hpp"

/// Boiler plate for defining memory comparision operations
#define MEMORY_OPERATION(op)                                \
  bool operator op##=(Int value)                            \
  {                                                         \
    Int register_value = this->operator Int();              \
    bool status = this->operator=(register_value op value); \
    return status;                                          \
  }

/// Boiler plate for defining restricted  operations for reserved memory
/// locations.
#define RESERVED_OPERATION(op)                                          \
  template <typename T>                                                 \
  constexpr bool operator op##=(T)                                      \
  {                                                                     \
    static_assert(CallCheck<T>::kValue,                                 \
                  "Reserved memory cannot be assigned or manipulated"); \
    return false;                                                       \
  }

namespace sjsu
{
/// Function prototype for a function that performs a write operation for a
/// system.
using WriteFnt = bool (*)(intptr_t, size_t, uint8_t *);

/// Function prototype for a function that performs a read operation for a
/// system.
using ReadFnt = void (*)(intptr_t, size_t, uint8_t *);

namespace device
{
/// Represents a register within the memory map of a device.
///
/// @tparam Int - the int type to represent this register. This can be uint8_t,
///               uint16_t, int32_t, etc.
/// @tparam endianess - is the register big or little endian
/// @tparam write - Write function associated with this type
/// @tparam read - Read function associated with this type
template <typename Int, Endian endianess, WriteFnt write, ReadFnt read>
SJ2_PACKED(struct)
Register_t
{
  /// Can swap from big to little indian.
  static uint32_t BigEndianessSwap(size_t size, uint32_t value)
  {
    value = __builtin_bswap32(value);
    value = value >> ((sizeof(value) - size) * 8);
    return value;
  }

  /// Handler for value assignment of the register. This will attempt to write
  /// use the write function to write to the register in the defined memory map.
  ///
  /// @return true - if the assignment was successful
  /// @return false - if the assignment was a failure
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
  // clang-format off
  MEMORY_OPERATION(^);
  // clang-format on
  MEMORY_OPERATION(+);
  MEMORY_OPERATION(-);
  MEMORY_OPERATION(*);
  MEMORY_OPERATION(/);
  MEMORY_OPERATION(>>);
  MEMORY_OPERATION(<<);

  // The purpose of the NOLINT comment is due to the fact clang-tidy is set
  // to force this operator to be explicit. But that allowing this object to
  // be implicitly converted allows it to work when assigned to integer types

  /// @return Int - when this register is accessed like an integer, this
  /// function will fetch and return the latest value of that register from the
  /// memory map.
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

/// Represents a block of memory in the memory map that is reserved and cannot
/// be accessed.
///
/// @tparam kLength - number of bytes in the reserved region
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
  // clang-format off
  RESERVED_OPERATION(^);
  // clang-format on
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

/// Represents a contiguous block of memory in the memory map.
///
/// @tparam Int - int representation
/// @tparam kLength - number of bytes in the array
/// @tparam write - write function
/// @tparam read - read function
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

  /// Accesses a register from the array via the index
  Register_t<Int, Endian::kLittle, write, read> & operator[](size_t index)
  {
    return member_[index];
  }

  // The purpose of the NOLINT comment is due to the fact clang-tidy is set
  // to force this operator to be explicit. But that allowing this object to
  // be implicitly converted allows it to work when assigned to integer types
  /// @tparam kSize - The size of the array to return the results into
  /// @return std::array<Int, kSize> - return the contents of the array back to
  /// a std::array object.
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

/// Device takes a MemoryMap structure and a Device communication protocol and
/// combines them to create a hardware abstraction that allows the remote device
/// to be accessed as if it were apart of the local memory map.
///
/// @tparam DeviceProtocol - Device communicate protocol
/// @tparam endianess - the endianess of the device to communicate with
/// @tparam MemoryMap - the memory map structure to use.
template <class DeviceProtocol,
          Endian endianess,
          template <Endian endian, WriteFnt write, ReadFnt read>
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

  static constexpr bool kWriteExists =
      (sizeof(TestWriteExists<DeviceProtocol>(0)) == sizeof(success));
  static constexpr bool kReadExists =
      (sizeof(TestReadExists<DeviceProtocol>(0)) == sizeof(success));

 public:
  /// Pointer to the memory map object. Such an object should never be
  /// constructed as it would take up a large amount of space.
  MemoryMap<endianess,
            Device::Write<DeviceProtocol>,
            Device::Read<DeviceProtocol>> & memory;

  /// MapType is a none-reference alias to the memory object
  using MapType = std::remove_reference_t<decltype(memory)>;

  /// The I2cDevice constructor's only job in this case is to set the memory
  /// ptr to 0. This will align the memory map to address zero. So when the
  /// registers lookup their own address location, what they are really getting
  /// is their position in the memory map.
  constexpr Device() : memory(*static_cast<MapType *>(0))
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

/// I2C Device handler for I2C device memory map structures.
///
/// @tparam kDeviceAddress - Device of the I2C device
/// @tparam endianess - The endianess of the I2C device
/// @tparam MemoryMap - The memory map structure of the device
template <const uint8_t kDeviceAddress,
          Endian endianess,
          template <Endian endian, WriteFnt write, ReadFnt read>
          class MemoryMap>
class I2cDevice : public Device<I2cDevice<kDeviceAddress, endianess, MemoryMap>,
                                endianess,
                                MemoryMap>
{
 public:
  /// Standard Write transaction for most I2C devices
  ///
  /// @param address - location to write to
  /// @param size - the number of bytes to send
  /// @param target - the data to write over I2C
  /// @return true - on successful write
  /// @return false - on failure to write
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

  /// Standard Read transaction for most I2C devices
  ///
  /// @param address - location to read from
  /// @param size - the number of bytes to read
  /// @param target - the buffer to read the I2C data into
  static void Read(intptr_t address, size_t size, uint8_t * target)
  {
    uint8_t register_address = static_cast<uint8_t>(address);
    i2c->WriteThenRead(kDeviceAddress, &register_address, 1, target, size);
  }

  /// Constructor of the I2cDevice
  ///
  /// @param i2c_peripheral - I2C peripheral implementation
  explicit I2cDevice(const I2c * i2c_peripheral)
  {
    i2c = i2c_peripheral;
  }

 private:
  inline static const I2c * i2c = nullptr;
};

#undef RESERVED_OPERATION
#undef MEMORY_OPERATION

}  // namespace sjsu

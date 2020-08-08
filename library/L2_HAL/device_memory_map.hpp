#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <type_traits>
#include <initializer_list>
#include <tuple>
#include <span>
#include <utility>

#include "L1_Peripheral/i2c.hpp"
#include "utility/enum.hpp"
#include "utility/macros.hpp"
#include "utility/status.hpp"
#include "utility/bit.hpp"
#include "utility/byte.hpp"

namespace sjsu
{
class DeviceMemoryMap
{
 public:
  // ===========================================================================
  // Interface Defintions
  // ===========================================================================

  /// Available sizes for addresses. Currently only supports 1, 2, 3, and 4 byte
  /// wide address spaces.
  enum class AddressWidth : uint8_t
  {
    Byte1 = 1,
    Byte2 = 2,
    Byte3 = 3,
    Byte4 = 4,
    // Always Keep "Maximum" this at the bottom of the list
    Maximum,
  };

  /// The maximum number of bytes to represent an address
  static constexpr size_t kAddressSizeLimit = Value(AddressWidth::Maximum) - 1;

  /// Specifies information about the device's memory
  struct MemorySpecification
  {
    /// Defines how many bytes are used to address memory. Typically for I2C
    /// devices they use 8-bits (or 1 byte) for memory addresses. Some devices
    /// have 32-bit address spaces thus they need 4 bytes.
    AddressWidth address_width;

    /// Defines the endianess of the system. Allows the device memory map to
    /// swap the endianness of whole integer values when they do not match the
    /// system's endianness.
    std::endian endianness;
  };

  struct RegisterPrototype
  {
    /// Address location as a number. Prefer to use a radix that matches the
    /// datasheet. For example, if the datasheet uses hex, use hexidecimal
    /// values. If the addresses are in decimal, use decimal to make
    /// verification easier.
    uint32_t address;

    /// How many bytes constitute the width of the register. This can be larger
    /// than the standard int types supported on the system (example: uint16_t,
    /// int32_t, uint64_t, etc), but they will not be retrievable via those
    /// return types. You will need to retreive them using the std::array<T,N>
    /// cast.
    size_t width;
  };

  /// Used to define the address and size of registers within a device
  /// Register is used with the DeviceMemoryMap's bracket operator `[]` in order
  /// to generate MemoryMapIO objects which can perform read and write
  /// operations on the register.
  template <MemorySpecification spec>
  struct Register
  {
    static constexpr MemorySpecification GetSpecification()
    {
      return spec;
    }

    /// Address location as a number. Prefer to use a radix that matches the
    /// datasheet. For example, if the datasheet uses hex, use hexidecimal
    /// values. If the addresses are in decimal, use decimal to make
    /// verification easier.
    std::array<uint8_t, 4> buffer;

    std::span<const uint8_t> address;

    /// How many bytes constitute the width of the register. This can be larger
    /// than the standard int types supported on the system (example: uint16_t,
    /// int32_t, uint64_t, etc), but they will not be retrievable via those
    /// return types. You will need to retreive them using the std::array<T,N>
    /// cast.
    size_t width;

    explicit constexpr Register(RegisterPrototype reg)
        : buffer{}, address{}, width(reg.width)
    {
      buffer             = ToByteArray(spec.endianness, reg.address);
      auto address_width = Value(spec.address_width);

      if (spec.endianness == std::endian::big)
      {
        address = std::span<const uint8_t>(&buffer.end()[-address_width],
                                           address_width);
      }
      else
      {
        address = std::span<const uint8_t>(buffer.begin(), address_width);
      }
    }
    constexpr Register() : buffer{}, address{}, width(0) {}
  };

  /// Utilizes a DeviceMemoryMap implementations in order to read and write to
  /// the register passed to it. It's usage is quite vast. It works by using
  /// implicit cast operations to detect what type it is being assigned to in
  /// order to read or write bytes from the device and store them in a variable.
  ///
  template <MemorySpecification spec>
  class MemoryMapIO
  {
   public:
    using ContainerInt = uint64_t;

    constexpr MemoryMapIO(DeviceMemoryMap & memory_map,
                          const Register<spec> & reg)
        : map_(memory_map), register_(reg)
    {
    }

    constexpr void Write(std::span<const uint8_t> payload)
    {
      map_.Write(register_.address, payload);
    }

    constexpr void Read(std::span<uint8_t> payload) const
    {
      map_.Read(register_.address, payload);
    }

    template <typename T>
    using IsInteger = std::enable_if_t<std::is_integral_v<T>>;

    template <typename Integer, typename = IsInteger<Integer>>
    void operator=(Integer write_value)
    {
      // Create short hand variables for the specification variables
      const auto width = std::min(register_.width, sizeof(ContainerInt));

      const auto endian     = spec.endianness;
      ContainerInt value    = static_cast<ContainerInt>(write_value);
      const auto bytes      = ToByteArray(endian, value);
      const auto value_span = ByteArrayToSpan(endian, bytes, width);

      Write(value_span);
    }

    template <size_t N>
    void operator=(const std::array<uint8_t, N> && value_array)
    {
      Write(value_array);
    }

    template <size_t N>
    void operator=(const std::array<uint8_t, N> & value_array)
    {
      Write(value_array);
    }

    template <typename T, size_t N>
    void operator=(const std::array<T, N> && value_array)
    {
      std::array<uint8_t, N * sizeof(T)> endian_payload;

      for (size_t i = 0; i < value_array.size(); i++)
      {
        auto byte_array = ToByteArray(spec.endianness, value_array[i]);
        std::copy(byte_array.begin(), byte_array.end(),
                  endian_payload.begin() + (i * byte_array.size()));
      }

      *this = endian_payload;
    }

    template <typename T, size_t N>
    void operator=(const std::array<T, N> & value_array)
    {
      *this = std::move(value_array);
    }

    template <typename Integer>
    operator Integer() const
    {
      std::array<uint8_t, sizeof(Integer)> read_buffer = { 0 };

      const auto width         = register_.width;
      const auto kRegisterSize = std::min(width, sizeof(ContainerInt));
      const std::span<uint8_t> read_span(read_buffer.begin(), kRegisterSize);

      Read(read_span);

      return ToInteger<Integer>(spec.endianness, read_span);
    }

    template <size_t N>
    operator std::array<uint8_t, N>() const
    {
      std::array<uint8_t, N> result;
      Read(result);
      return result;
    }

    template <typename T, size_t N>
    operator std::array<T, N>() const
    {
      std::array<uint8_t, N * sizeof(T)> byte_buffer = *this;
      return ToIntegerArray<T, N>(spec.endianness, byte_buffer);
    }

/// Boiler plate for defining memory comparision operations
#define ADD_ARITHMETIC_OPERATION(op)                    \
  template <typename T>                                 \
  friend T operator op(const MemoryMapIO & reg, T mask) \
  {                                                     \
    T value = reg;                                      \
    return static_cast<T>(value op mask);               \
  }                                                     \
  template <typename T>                                 \
  friend T operator op(T mask, const MemoryMapIO & reg) \
  {                                                     \
    T value = reg;                                      \
    return static_cast<T>(value op mask);               \
  }                                                     \
  template <typename T>                                 \
  MemoryMapIO & operator op##=(T mask)                  \
  {                                                     \
    T original = *this;                                 \
    T value    = static_cast<T>(original + mask);       \
    *this      = value;                                 \
    return *this;                                       \
  }

    ADD_ARITHMETIC_OPERATION(|);
    ADD_ARITHMETIC_OPERATION(&);
    ADD_ARITHMETIC_OPERATION(^);
    ADD_ARITHMETIC_OPERATION(+);
    ADD_ARITHMETIC_OPERATION(-);
    ADD_ARITHMETIC_OPERATION(*);
    ADD_ARITHMETIC_OPERATION(/);
    ADD_ARITHMETIC_OPERATION(>>);
    ADD_ARITHMETIC_OPERATION(<<);

#undef ADD_ARITHMETIC_OPERATION

   private:
    // Reference to a DeviceMemoryMap object which implements the write and read
    // methods.
    DeviceMemoryMap & map_;

    // Contains the register size and address
    const Register<spec> & register_;
  };

  // ===========================================================================
  // Interface Methods
  // ===========================================================================

  virtual void Write(std::span<const uint8_t> address,
                     std::span<const uint8_t> payload) = 0;
  virtual void Read(std::span<const uint8_t> address,
                    std::span<uint8_t> payload)        = 0;

  // ===========================================================================
  // Class Methods
  // ===========================================================================

  template <MemorySpecification spec>
  auto operator[](const DeviceMemoryMap::Register<spec> & device_register)
  {
    return MemoryMapIO<spec>(*this, device_register);
  }

  /// @return the error object
  Error_t GetError()
  {
    return error_;
  }

  /// Drivers using this class can clear an error flag by using this method.
  void ClearError()
  {
    error_.status = Status::kSuccess;
  }

 protected:
  /// Implementations of this class must use this method when error are
  /// detected. This allows drivers using this class to know if an error has
  /// occurred by checking the `ErrorOccurred()` method.
  void SetError(Error_t error)
  {
    error_ = error;
  }

 private:
  Error_t error_;
};  // namespace sjsu

// TODO(#1330): Not implemented yet
template <DeviceMemoryMap::AddressWidth size>
class MockDeviceMap : public DeviceMemoryMap
{
};

/// Size and time efficient MockDeviceMap replacement for unit testing. Prefer
/// this to the other varients when the address size is only 1 byte.
template <>
class MockDeviceMap<DeviceMemoryMap::AddressWidth::Byte1>
    : public DeviceMemoryMap
{
 public:
  MockDeviceMap() {}

  std::array<uint8_t, (1 << 8)> memory_map;

  void Write(std::span<const uint8_t> address,
             std::span<const uint8_t> payload) override
  {
    // Only use the first byte of the address
    std::copy_n(payload.begin(), payload.size(),
                memory_map.begin() + address[0]);
  }

  void Read(std::span<const uint8_t> address,
            std::span<uint8_t> payload) override
  {
    // Only use the first byte of the address
    std::copy_n(memory_map.begin() + address[0], payload.size(),
                payload.begin());
  }

  void UnitTestSetError(Error_t error)
  {
    SetError(error);
  }
};

// TODO(#1329) Not supported yet.
/// Size and time efficient MockDeviceMap replacement for unit testing. Prefer
/// this to the other varients when the address size is only 1 byte.
template <>
class MockDeviceMap<DeviceMemoryMap::AddressWidth::Byte2>
    : public DeviceMemoryMap
{
  std::array<uint8_t, (1 << 16)> memory_map;
};

template <size_t MaximumPayloadSize = 1>
class I2cMemory : public DeviceMemoryMap
{
 public:
  constexpr I2cMemory(uint8_t i2c_address, const sjsu::I2c & i2c)
      : i2c_address_(i2c_address), i2c_(i2c)
  {
  }

  void Write(std::span<const uint8_t> address,
             std::span<const uint8_t> value) override
  {
    constexpr auto kAddressSize = DeviceMemoryMap::kAddressSizeLimit;
    constexpr auto kBufferSize  = kAddressSize + MaximumPayloadSize;

    if (address.size() + value.size() >= kBufferSize)
    {
      SetError(
          Error_t(Status::kOutOfBounds,
                  "I2cMemory Object does not have enough buffer storage to "
                  "perform this write operation."));
      return;
    }

    std::array<uint8_t, kBufferSize> buffer;

    std::copy(address.begin(), address.end(), buffer.begin());
    std::copy(value.begin(), value.end(), buffer.begin() + address.size());

    auto result = i2c_.Write(i2c_address_, buffer);

    if (!result)
    {
      SetError(result.error());
    }
  }

  void Read(std::span<const uint8_t> address,
            std::span<uint8_t> receive) override
  {
    auto result = i2c_.WriteThenRead(i2c_address_, address, receive);

    if (!result)
    {
      SetError(result.error());
    }
  }

 private:
  uint8_t i2c_address_;
  const sjsu::I2c & i2c_;
};

// constexpr bool NoRegistersOverlap(
//     DeviceMemoryMap::MemorySpecification spec,
//     std::initializer_list<DeviceMemoryMap::Register> registers)
// {
//   uint32_t current_address = 0;

//   for (const auto & reg : registers)
//   {
//     auto register_address =
//     ToInteger<uint32_t><spec.endianness>(reg.address); if (register_address <
//     current_address)
//     {
//       return false;
//     }
//     current_address = register_address + reg.width;
//   }
//   return true;
// }

#define SJ2_CHECK_DEVICE_ERROR(device_object) \
  {                                           \
    if (device_object.GetError())             \
    {                                         \
      auto error_value = device_.GetError();  \
      device_.ClearError();                   \
      return Error(std::move(error_value));   \
    }                                         \
  }

}  // namespace sjsu

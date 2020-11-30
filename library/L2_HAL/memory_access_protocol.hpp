#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <span>
#include <utility>

#include "L1_Peripheral/i2c.hpp"
#include "utility/enum.hpp"
#include "utility/error_handling.hpp"
#include "utility/byte.hpp"

namespace sjsu
{
class MemoryAccessProtocol
{
 public:
  // ===========================================================================
  // Interface Defintions
  // ===========================================================================

  /// Available sizes for addresses. Currently only supports 1, 2, 3, and 4 byte
  /// wide address spaces.
  enum class AddressWidth : uint8_t
  {
    kByte1 = 1,
    kByte2 = 2,
    kByte3 = 3,
    kByte4 = 4,
    // Always Keep "Maximum" this at the bottom of the list
    kMaximum,
  };

  /// The maximum number of bytes to represent an address
  static constexpr size_t kAddressSizeLimit = Value(AddressWidth::kMaximum) - 1;

  /// Specifies information about the device's memory
  ///
  /// @tparam  Defines how many bytes are used to address memory. Typically for
  /// I2C devices they use 8-bits (or 1 byte) for memory addresses. Some devices
  /// have 32-bit address spaces thus they need 4 bytes.
  /// @tparam Defines the endianess of the system. Allows the device memory map
  /// to swap the endianness of whole integer values when they do not match the
  /// system's endianness.
  template <AddressWidth address_width, std::endian endianness>
  struct Specification_t
  {
    static constexpr auto AddressWidth()
    {
      return address_width;
    }
    static constexpr auto Endianness()
    {
      return endianness;
    }
  };

  struct AddressLocationPrototype_t
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
    uint8_t width;
  };

  /// Used to define the address and size of registers within a device
  /// Register is used with the MemoryAccessProtocol's bracket operator `[]` in
  /// order to generate AccessHandler objects which can perform read and write
  /// operations on the register.
  template <AddressWidth address_width, std::endian endianness>
  class Address
  {
   public:
    static constexpr auto GetSpecification()
    {
      return Specification_t<address_width, endianness>{};
    }

    static constexpr auto AddressWidth()
    {
      return Value(GetSpecification().AddressWidth());
    }

    static constexpr auto Endianess()
    {
      return GetSpecification().Endianness();
    }

    /// Address location as a number. Prefer to use a radix that matches the
    /// datasheet. For example, if the datasheet uses hex, use hexidecimal
    /// values. If the addresses are in decimal, use decimal to make
    /// verification easier.
    std::array<uint8_t, AddressWidth()> address;

    /// How many bytes constitute the width of the Address. This can be larger
    /// than the standard int types supported on the system (example: uint16_t,
    /// int32_t, uint64_t, etc), but they will not be retrievable via those
    /// return types. You will need to retreive them using the std::array<T,N>
    /// cast.
    uint8_t width;

    explicit constexpr Address(Specification_t<address_width, endianness>,
                               AddressLocationPrototype_t proto)
        : address{}, width(proto.width)
    {
      address = ToByteArray<decltype(proto.address), AddressWidth()>(
          Endianess(), proto.address);
    }
  };

  /// Utilizes a MemoryAccessProtocol implementations in order to read and write
  /// to the register passed to it. It's usage is quite vast. It works by using
  /// implicit cast operations to detect what type it is being assigned to in
  /// order to read or write bytes from the device and store them in a variable.
  template <AddressWidth address_width, std::endian endianness>
  class AccessHandler
  {
   public:
    using ContainerInt = uint64_t;

    constexpr AccessHandler(MemoryAccessProtocol & memory_map,
                            const Address<address_width, endianness> & address)
        : map_(memory_map), address_(address)
    {
    }

    constexpr void Write(std::span<const uint8_t> payload)
    {
      return map_.Write(address_.address, payload);
    }

    constexpr void Read(std::span<uint8_t> payload) const
    {
      return map_.Read(address_.address, payload);
    }

    template <typename T>
    using IsInteger = std::enable_if_t<std::is_integral_v<T>>;

    template <typename Integer, typename = IsInteger<Integer>>
    void operator=(Integer write_value)
    {
      // Create short hand variables for the specification variables
      const ContainerInt kValue = static_cast<ContainerInt>(write_value);
      const auto kEndian        = endianness;
      const size_t kWidth       = address_.width;
      const auto kRegisterSize  = std::min(kWidth, sizeof(ContainerInt));
      const auto kBytes         = ToByteArray(kEndian, kValue);
      const auto kValueSpan = ByteArrayToSpan(kEndian, kBytes, kRegisterSize);

      return Write(kValueSpan);
    }

    template <size_t N>
    void operator=(const std::array<uint8_t, N> && value_array)
    {
      return Write(value_array);
    }

    template <size_t N>
    void operator=(const std::array<uint8_t, N> & value_array)
    {
      return Write(value_array);
    }

    template <typename T, size_t N>
    void operator=(const std::array<T, N> && value_array)
    {
      std::array<uint8_t, N * sizeof(T)> endian_payload;

      for (size_t i = 0; i < value_array.size(); i++)
      {
        auto byte_array = ToByteArray(endianness, value_array[i]);
        std::copy(byte_array.begin(), byte_array.end(),
                  endian_payload.begin() + (i * byte_array.size()));
      }

      return (*this = endian_payload);
    }

    template <typename T, size_t N>
    void operator=(const std::array<T, N> & value_array)
    {
      return (*this = std::move(value_array));
    }

    template <typename Integer>
    operator Integer() const
    {
      std::array<uint8_t, sizeof(ContainerInt)> read_buffer = { 0 };

      const size_t kWidth      = address_.width;
      const auto kRegisterSize = std::min(kWidth, sizeof(ContainerInt));
      const std::span<uint8_t> kReadSpan(read_buffer.begin(), kRegisterSize);

      Read(kReadSpan);

      return ToInteger<Integer>(endianness, kReadSpan);
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
      return ToIntegerArray<T, N>(endianness, byte_buffer);
    }

/// Boiler plate for defining memory comparision operations
#define ADD_ARITHMETIC_OPERATION(op)                             \
  template <typename T>                                          \
  friend T operator op(const AccessHandler & reg, T local_value) \
  {                                                              \
    T memory_value = reg;                                        \
    return static_cast<T>(memory_value op local_value);          \
  }                                                              \
                                                                 \
  template <typename T>                                          \
  friend T operator op(T local_value, const AccessHandler & reg) \
  {                                                              \
    T memory_value = reg;                                        \
    return static_cast<T>(memory_value op local_value);          \
  }                                                              \
                                                                 \
  template <typename T>                                          \
  AccessHandler & operator op##=(T local_value)                  \
  {                                                              \
    T original  = *this;                                         \
    T new_value = static_cast<T>(original op local_value);       \
    *this       = new_value;                                     \
    return *this;                                                \
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
    // Reference to a MemoryAccessProtocol object which implements the write and
    // read methods.
    MemoryAccessProtocol & map_;

    // Contains the register size and address
    const Address<address_width, endianness> & address_;
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

  template <AddressWidth address_width, std::endian endianness>
  auto operator[](
      const MemoryAccessProtocol::Address<address_width, endianness> &
          device_register)
  {
    return AccessHandler(*this, device_register);
  }
};  // namespace sjsu

// TODO(#1330): Not implemented yet
template <MemoryAccessProtocol::AddressWidth size>
class MockProtocol : public MemoryAccessProtocol
{
};

/// Size and time efficient MockProtocol replacement for unit testing. Prefer
/// this to the other varients when the address size is only 1 byte.
template <>
class MockProtocol<MemoryAccessProtocol::AddressWidth::kByte1>
    : public MemoryAccessProtocol
{
 public:
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
};

// TODO(#1329) Not supported yet.
/// Size and time efficient MockProtocol replacement for unit testing. Prefer
/// this to the other varients when the address size is only 1 byte.
template <>
class MockProtocol<MemoryAccessProtocol::AddressWidth::kByte2>
    : public MemoryAccessProtocol
{
  std::array<uint8_t, (1 << 16)> memory_map_;
};

template <size_t MaximumPayloadSize = 1>
class I2cProtocol : public MemoryAccessProtocol
{
 public:
  constexpr I2cProtocol(uint8_t i2c_address, sjsu::I2c & i2c)
      : i2c_address_(i2c_address), i2c_(i2c)
  {
  }

  void Write(std::span<const uint8_t> address,
             std::span<const uint8_t> value) override
  {
    constexpr auto kAddressSize = MemoryAccessProtocol::kAddressSizeLimit;
    constexpr auto kBufferSize  = kAddressSize + MaximumPayloadSize;

    if (address.size() + value.size() >= kBufferSize)
    {
      throw Exception(
          std::errc::not_enough_memory,
          "I2cProtocol Object does not have enough buffer storage to "
          "perform this write operation.");
    }

    std::array<uint8_t, kBufferSize> buffer;

    std::copy(address.begin(), address.end(), buffer.begin());
    std::copy(value.begin(), value.end(), buffer.begin() + address.size());

    i2c_.Write(i2c_address_, buffer);
  }

  void Read(std::span<const uint8_t> address,
            std::span<uint8_t> receive) override
  {
    i2c_.WriteThenRead(i2c_address_, address, receive);
  }

 private:
  uint8_t i2c_address_;
  sjsu::I2c & i2c_;
};

template <MemoryAccessProtocol::AddressWidth address_width,
          std::endian endianness>
constexpr bool NoRegistersOverlap(
    std::initializer_list<
        MemoryAccessProtocol::Address<address_width, endianness>> addresses)
{
  uint32_t current_address_location = 0;

  for (const auto & address : addresses)
  {
    auto address_integer = ToInteger<uint32_t>(endianness, address.address);
    if (address_integer < current_address_location)
    {
      return false;
    }
    current_address_location = address_integer + address.width;
  }
  return true;
}
}  // namespace sjsu

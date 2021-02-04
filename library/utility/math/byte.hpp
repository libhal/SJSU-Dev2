#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <climits>
#include <cstdint>
#include <span>

namespace sjsu
{
/// Convert a numeric value into an array of bytes.
///
/// @tparam T - value to be converted into an array of bytes
/// @tparam array_size - the size of the resultant array. This can be bigger
/// than the size of the integer/value type. Just like how a 2 byte integer can
/// fit in a 8 byte register.
/// @param endian - the endianness of the byte array.
/// @param value - the value to be converted into an array
/// @return constexpr auto - the std::array<> containing the value in bytes the
/// size of array_size.
template <typename T, size_t array_size = sizeof(T)>
constexpr auto ToByteArray(std::endian endian, T value)
{
  std::array<uint8_t, array_size> array = {};
  static_assert(std::is_integral_v<T>,
                "Type T (the return type) must be intergal type.");
  if constexpr (std::is_integral_v<T>)
  {
    if (endian == std::endian::little)
    {
      for (size_t i = 0; i < array.size(); i++)
      {
        auto next_byte   = value >> ((CHAR_BIT * i) & 0xFF);
        array.begin()[i] = static_cast<uint8_t>(next_byte);
      }
    }
    else
    {
      for (size_t i = 0; i < array.size(); i++)
      {
        auto next_byte    = value >> ((CHAR_BIT * i) & 0xFF);
        array.rbegin()[i] = static_cast<uint8_t>(next_byte);
      }
    }
  }
  return array;
}

/// Convert an array into a span based on its endianness. Useful for picking out
/// values within an array that
///
/// @tparam N - number of bytes within the array. Do not set this value, it is
/// inferred by the passed in std::array.
/// @param endian - the endianness of the subspan.
/// @param bytes - array containing the bytes to be converted into a span.
/// @param width - the width of the integer contents within array. This value
/// must be 1 or equal to `N` (size of the array). Any lower or greater and the
/// consequences are undefined.
/// @return constexpr auto - std::span of the array based on the endianness.
/// Note that a span is non-owning, meaning that it will point to invalid data
/// if the passed in bytes is no longer valid.
template <size_t N>
constexpr auto ByteArrayToSpan(std::endian endian,
                               const std::array<uint8_t, N> & bytes,
                               size_t width = N)
{
  if (endian == std::endian::big)
  {
    return std::span<const uint8_t>(&bytes.end()[-width], width);
  }
  else
  {
    return std::span<const uint8_t>(bytes.data(), width);
  }
}

/// Convert an array/span into an integer with respect to endianness.
///
/// @tparam T - the integer type that the array will be assembled into
/// @param endian - endianness of the input array
/// @param array - the array of bytes to be assembled into a integer.
/// @return constexpr auto - the converted value.
template <typename T>
constexpr auto ToInteger(std::endian endian, std::span<const uint8_t> array)
{
  static_assert(std::is_integral_v<T>,
                "Type T (the return type) must be intergal type.");
  T value = 0;
  if constexpr (std::is_integral_v<T>)
  {
    size_t end = std::min(sizeof(T), array.size());

    if (endian == std::endian::little)
    {
      for (size_t i = 0; i < end; i++)
      {
        auto or_value = static_cast<T>(array.begin()[i]) << (CHAR_BIT * i);
        value         = static_cast<T>(value | or_value);
      }
    }
    else
    {
      for (size_t i = 0; i < end; i++)
      {
        auto or_value = static_cast<T>(array.rbegin()[i]) << (CHAR_BIT * i);
        value         = static_cast<T>(value | or_value);
      }
    }
  }

  return value;
}

/// Like ToInteger, except that, it converts an array of bytes into an array of
/// Integers.
///
/// @tparam T - the integer type that the array will be assembled into
/// @tparam N - number of integers to convert.
/// @param endian - endianness of the bytes array
/// @param bytes - array of bytes to be converted into an array of integers.
/// @return constexpr auto - array of the converted values.
template <typename T, size_t N>
constexpr auto ToIntegerArray(std::endian endian,
                              std::span<const uint8_t> bytes)
{
  static_assert(std::is_integral_v<T>,
                "Type T (the return type) must be intergal type.");
  std::array<T, N> value = { 0 };
  std::span<const uint8_t> byte_span;

  if constexpr (std::is_integral_v<T>)
  {
    for (size_t i = 0; i < bytes.size(); i += sizeof(T))
    {
      byte_span            = bytes.subspan(i, sizeof(T));
      value[i / sizeof(T)] = ToInteger<T>(endian, byte_span);
    }
  }
  return value;
}
}  // namespace sjsu

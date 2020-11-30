#pragma once

#include <algorithm>
#include <array>
#include <bit>
#include <climits>
#include <cstdint>
#include <span>

namespace sjsu
{
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

template <size_t N>
constexpr auto ByteArrayToSpan(std::endian endian,
                               const std::array<uint8_t, N> & bytes,
                               size_t width)
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

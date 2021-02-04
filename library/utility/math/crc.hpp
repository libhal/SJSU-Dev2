#pragma once

#include <cstdlib>
#include <cstdint>
#include <type_traits>

namespace sjsu
{
namespace crc
{
/// A template housing a CRC7 or CRC16 table for the SD card, using either an
/// 8-bit or 16-bit data type.
template <typename T>
struct CrcTableConfig_t
{
  /// Maximum table size.
  static constexpr size_t kTableSize   = 256;
  /// 16-bit polynomial value used for calculating the CRC.
  static constexpr uint32_t kPoly16bit = (1 << 16 | 1 << 12 | 1 << 5) + 1;
  /// 8-bit polynomial value used for calculating the CRC.
  static constexpr uint8_t kPoly8bit   = 0x89;

  // Compile-time check for appropriate crc table type
  static_assert(
      std::is_same<T, uint16_t>::value || std::is_same<T, uint8_t>::value,
      "Incompatibility: SD CRC table only accepts uint8_t or uint16_t");
  /// Table of 16 or 8 bit CRC lookup values.
  T crc_table[kTableSize] = { 0 };
};

/// This function acts as a compile-time CRC-7 table generator.
/// @tparam T size of each element in the table
/// @return constexpr CrcTableConfig_t<T> The configured CRC-7 table container.
template <typename T>
constexpr static CrcTableConfig_t<T> GenerateCrc7Table()
{
  CrcTableConfig_t<T> crc_table = CrcTableConfig_t<T>();
  size_t i = 0, j = 0;
  // generate a table value for all 256 possible byte values
  for (i = 0; i < 0xFF; i++)
  {
    bool most_significant_bit_set = static_cast<bool>(i & 0x80);
    uint8_t polynomial_compare = static_cast<uint8_t>(i) ^ crc_table.kPoly8bit;

    crc_table.crc_table[i] = (most_significant_bit_set)
                                 ? static_cast<T>(polynomial_compare)
                                 : static_cast<T>(i);

    for (j = 1; j < 8; j++)
    {
      crc_table.crc_table[i] = static_cast<T>(crc_table.crc_table[i] << 1);
      if (crc_table.crc_table[i] & 0x80)
      {
        crc_table.crc_table[i] ^= crc_table.kPoly8bit;
      }
    }
  }
  return crc_table;
}

/// This function acts as a compile-time CRC-16 table generator.
/// @returns The configured CCITT CRC-16 table container.
constexpr static CrcTableConfig_t<uint16_t> GenerateCrc16Table()
{
  // Create a place to store our table data
  CrcTableConfig_t<uint16_t> table;

  for (uint16_t i = 0; i < 256; i++)
  {
    // Initialize the CRC for this byte
    uint16_t crc = static_cast<uint16_t>(i << 8);
    // Check each bit
    for (uint16_t j = 0; j < 8; j++)
    {
      // Check if we will need to add the polynomial with no carry
      // (i.e. XOR it)
      bool xor_is_needed = crc & (1 << 15);
      // Left-shift the crc
      crc = static_cast<uint16_t>(crc << 1);
      // Check if msb is 1
      if (xor_is_needed)
      {
        // XOR it with the polynomial
        crc ^= static_cast<uint16_t>(table.kPoly16bit);
      }
    }
    // Add crc to the table
    table.crc_table[i] = crc;
  }
  return table;
}
}  // namespace crc
}  // namespace sjsu

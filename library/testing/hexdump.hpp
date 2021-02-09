#pragma once

#include "testing/testing_frameworks.hpp"

/// Helper class used as a template specialization for doctest::StringMaker to
/// print any data type as a hexdump.
template <class Data>
struct HexDumpObject
{
  /// @param data - data to be displayed in a hexdump
  HexDumpObject(const Data & data_to_hex) : data(data_to_hex) {}

  // Reference to the data to be hexdumped.
  const Data & data;
};

namespace doctest
{
/// Template specialization that turns a data structure into a hexdump string.
template <class PeripheralStructure>
struct StringMaker<sjsu::HexDumpObject<PeripheralStructure>>
{
  /// Convert hexdump data to a string that doctest can use.
  static String convert(
      const sjsu::HexDumpObject<PeripheralStructure> & dump)  // NOLINT
  {
    auto array_of_text = sjsu::debug::HexdumpStructure(dump.data);
    std::string str(array_of_text.data(), array_of_text.size());
    String result(str.data(), str.size());
    return result;
  }
};
}  // namespace doctest

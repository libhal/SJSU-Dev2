#pragma once

#include <string>

#include "testing/testing_frameworks.hpp"
#include "utility/debug.hpp"

namespace sjsu
{
/// Helper class used as a template specialization for doctest::StringMaker to
/// print any data type as a hexdump.
template <class Data>
struct HexDumpObject
{
  /// @param data_to_hex - data to be displayed in a hexdump if used via doctest
  /// StringMaker.
  explicit HexDumpObject(const Data & data_to_hex) : data(data_to_hex) {}

  /// Reference to the data to be hexdumped.
  const Data & data;
};

/// Helper class for comparing manipulations made to a peripheral's memory
/// against what is expected after a driver methods have been called.
/// Normally, the procedure for simulating hardware in a host test is to setup
/// the memory as expected before a function call, call the function, then
/// afterwards check each field that they expect to be modified. The problem
/// with this approach is that leaves out all of the other fields that may have
/// been incorrectly manipulated by a function or class method. One could write
/// code to check the rest but this tedious and could be forgotten.
///
/// This class simplifies the testing procedure, reduces the amount of code
/// written and most importantly performs a wholistic comparison between the
/// mock memory and the expected memory, ensuring that any bit of data that is
/// not set correctly is checked for.
///
/// Usage:
///
///     AutoVerifyPeripheralMemory adc_memory(&Adc::adc_base);
///
///     // Modify the expected memory
///     bit::Register(&mock_peripheral.Expected()->CR)
///         .Insert(kExpectedDivider, Adc::Control::kClockDivider)
///         .Set(Adc::Control::kPowerEnable)
///         .Set(Adc::Control::kBurstEnable)
///         .Set(bit::MaskFromRange(kMockChannel0.channel))
///         .Set(bit::MaskFromRange(kMockChannel1.channel))
///         .Save();
///
///     // Call function here...
///
///     // Let the destructor of adc_memory handle checking the ADC memory
///
/// @tparam Peripheral - Peripheral data structure type like LPC40xx
/// LPC_ADC_TypeDef.
template <class Peripheral>
class AutoVerifyPeripheralMemory
{
 public:
  /// @param peripheral_address  - pointer to the peripheral pointer used in the
  /// driver. This will save the previous peripheral address, replace it with
  /// the mock memory, and on destruction of the class, will return the
  /// peripheral memory pointer back to its original address.
  explicit AutoVerifyPeripheralMemory(Peripheral ** peripheral_address)
      : peripheral_address_(peripheral_address),
        original_address_(*peripheral_address),
        mock_memory_{},
        expected_memory_{}
  {
    // Set mock memory as the memory address for the peripheral
    *peripheral_address = &mock_memory_;

    // Set memory structures to all zeros.
    testing::ClearStructure(&mock_memory_);
    testing::ClearStructure(&expected_memory_);
  }

  /// @return a pointer to the mock memory. This can be used to setup the memory
  /// of the peripheral prior to callling a function that will manipulate it.
  /// For example, setting status flags such that code does not loop or setting
  /// up bits that need to be cleared later by the function to be called.
  Peripheral * Mock()
  {
    return &mock_memory_;
  }

  /// @return a pointer to the expected memory. Use this to setup the exected
  /// results of the memory after a function has been called.
  Peripheral * Expected()
  {
    return &expected_memory_;
  }

  /// @return a bit::Register to a member within the expected memory. Use this
  /// to setup the exected results of the memory after a function has been
  /// called.
  template <typename PeripheralMemoryType>
  bit::Register<PeripheralMemoryType> ExpectedRegister(
      volatile PeripheralMemoryType Peripheral::*member)
  {
    bit::Register expected_register(&(expected_memory_.*member));
    return expected_register;
  }

  /// Copy the mock memory to the expected memory. This can be used for many
  /// purposes. The main two purposes are:
  ///
  /// 1. After setting up mock memory, rather than using the same code again for
  /// expected, simply copy the data then make the last modifications to the
  /// expected memory.
  ///
  /// 2. To ensure that the auto validation always succeeds. Can be used to skip
  /// validation.
  void CopyMockToExpected()
  {
    memcpy(&expected_memory_, &mock_memory_, sizeof(mock_memory_));
  }

  ~AutoVerifyPeripheralMemory()
  {
    int memory_compare =
        memcmp(&mock_memory_, &expected_memory_, sizeof(mock_memory_));

    INFO("Expected: \n" << HexDumpObject(expected_memory_));
    INFO("Actual: \n" << HexDumpObject(mock_memory_));

    CHECK_MESSAGE(memory_compare == 0,
                  "The actual peripheral memory does not match expected "
                  "peripheral memory.");

    // Restore original peripheral address
    *peripheral_address_ = original_address_;
  }

 protected:
  /// Address of the driver's address pointer
  Peripheral ** peripheral_address_;
  /// The original address pointed to by the peripheral pointer
  Peripheral * original_address_;
  /// Mock memory that will be manipulated
  Peripheral mock_memory_;
  /// Expected memory that the test case manipulates and is compared to the mock
  /// memory.
  Peripheral expected_memory_;
};
}  // namespace sjsu

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

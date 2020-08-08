#include <numeric>
#include <string>

#include "L2_HAL/device_memory_map.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
// Setup
// Setup: Create a memory specification object based on the current
// iterations address width and endianness.
constexpr DeviceMemoryMap::MemorySpecification spec1 = {
  .address_width = DeviceMemoryMap::AddressWidth::Byte1,
  .endianness    = std::endian::big,
};

constexpr DeviceMemoryMap::MemorySpecification spec2 = {
  .address_width = DeviceMemoryMap::AddressWidth::Byte2,
  .endianness    = std::endian::big,
};

constexpr DeviceMemoryMap::MemorySpecification spec3 = {
  .address_width = DeviceMemoryMap::AddressWidth::Byte3,
  .endianness    = std::endian::big,
};

constexpr DeviceMemoryMap::MemorySpecification spec4 = {
  .address_width = DeviceMemoryMap::AddressWidth::Byte4,
  .endianness    = std::endian::big,
};

constexpr DeviceMemoryMap::MemorySpecification spec5 = {
  .address_width = DeviceMemoryMap::AddressWidth::Byte1,
  .endianness    = std::endian::little,
};

constexpr DeviceMemoryMap::MemorySpecification spec6 = {
  .address_width = DeviceMemoryMap::AddressWidth::Byte2,
  .endianness    = std::endian::little,
};

constexpr DeviceMemoryMap::MemorySpecification spec7 = {
  .address_width = DeviceMemoryMap::AddressWidth::Byte3,
  .endianness    = std::endian::little,
};

constexpr DeviceMemoryMap::MemorySpecification spec8 = {
  .address_width = DeviceMemoryMap::AddressWidth::Byte4,
  .endianness    = std::endian::little,
};

TEST_CASE_TEMPLATE("Testing DeviceMemoryMap",
                   TestRegister,
                   DeviceMemoryMap::Register<spec1>,
                   DeviceMemoryMap::Register<spec2>,
                   DeviceMemoryMap::Register<spec3>,
                   DeviceMemoryMap::Register<spec4>,
                   DeviceMemoryMap::Register<spec5>,
                   DeviceMemoryMap::Register<spec6>,
                   DeviceMemoryMap::Register<spec7>,
                   DeviceMemoryMap::Register<spec8>)
{
  // Top Level Setup:
  // Each byte of the expected value and address are unique to distinguish
  // between them in the test array fields.
  static constexpr size_t kArraySize     = 16;
  static constexpr uint32_t kTestAddress = 0x0A'0B'0C'0E;
  static constexpr uint64_t kTestValue   = 0x01'02'03'04'05'06'07'08;

  class UnitTestDeviceMemoryMap : public DeviceMemoryMap
  {
   public:
    constexpr UnitTestDeviceMemoryMap()
    {
      std::iota(read_value.begin(), read_value.end(), 0);
    }

    void Write(std::span<const uint8_t> address,
               std::span<const uint8_t> payload) override
    {
      write_address.fill(0);
      write_value.fill(0);

      write_address_span = address;
      write_value_span   = payload;

      std::copy(address.begin(), address.end(), write_address.begin());
      std::copy(payload.begin(), payload.end(), write_value.begin());
    }

    void Read(std::span<const uint8_t> address,
              std::span<uint8_t> payload) override
    {
      read_address.fill(0);

      read_value_span   = payload;
      read_address_span = address;

      std::copy(address.begin(), address.end(), read_address.begin());
      std::copy_n(read_value.begin(), payload.size(), payload.begin());
    }

    /// Used to setup the read_value
    void SetReadValue(std::span<uint8_t> new_read_value)
    {
      read_value.fill(0);
      std::copy(new_read_value.begin(), new_read_value.end(),
                read_value.begin());
    }

    std::array<uint8_t, kArraySize> write_address = {};
    std::array<uint8_t, kArraySize> write_value   = {};
    std::span<const uint8_t> write_address_span;
    std::span<const uint8_t> write_value_span;

    // This array is filled with content
    std::array<uint8_t, kArraySize> read_value   = {};
    std::array<uint8_t, kArraySize> read_address = {};
    std::span<const uint8_t> read_value_span;
    std::span<const uint8_t> read_address_span;
  };

  UnitTestDeviceMemoryMap mock_map;

  int test_iteration_counter = 0;

  // Check all valid value widths for integer assignment. The maximum
  // supported value assignment is 64-bits which is 8 bytes.
  for (const size_t & register_width : { 1, 2, 3, 4, 5, 6, 7, 8 })
  {
    test_iteration_counter++;

    auto spec          = TestRegister::GetSpecification();
    auto endian        = spec.endianness;
    auto address_width = Value(spec.address_width);

    // Setup: Extract the bits from the start to the number of bits
    // within the address width. For example, for a 3 byte address
    // width, the bits extracted would be [0:23]
    const uint32_t kTruncatedTestAddress = bit::Extract(
        kTestAddress, bit::MaskFromRange(0, (address_width * 8) - 1));

    // Setup: Create a test register.
    auto test_register = TestRegister({
        .address = kTruncatedTestAddress,
        .width   = register_width,
    });

    // Setup: Convert the truncated test address into a byte array,
    // based on the endianness
    std::array<uint8_t, sizeof(kTestAddress)> expected_address_bytes =
        ToByteArray<uint32_t>(endian, kTruncatedTestAddress);

    // Create a few arrays that will contain the expected address, value
    // and full payload data.
    std::array<uint8_t, kArraySize> expected_address = { 0 };

    // BIG endian has all of the bytes at the back of the array, and
    // thus, we need to find the start of the valid data in the array,
    // then copy the bytes in.
    if (endian == std::endian::big)
    {
      // Copy address_bytes into expected_address
      std::copy_n(&expected_address_bytes.end()[-address_width], address_width,
                  expected_address.begin());
    }
    else
    {
      // Copy address_bytes into expected_address
      std::copy_n(expected_address_bytes.begin(), address_width,
                  expected_address.begin());
    }

    // Create logs for when there are failures
    auto endian_string = ((endian == std::endian::big) ? "BIG" : "LITTLE");
    INFO("Endianness       = " << endian_string);
    INFO("Address Width    = " << static_cast<int>(spec.address_width));
    INFO("Register Width   = " << register_width);

    // Check all valid integer assignment types
    for (const size_t & value_width : { sizeof(uint8_t), sizeof(uint16_t),
                                        sizeof(uint32_t), sizeof(uint64_t) })
    {
      test_iteration_counter++;

      // Setup: Extract the bits from the start to the number of bits
      // within the value assignment width. For example, for a uint32_t (4
      // byte) value, the bits extracted would be [0:31]
      const size_t kTestValueWidth     = std::min(register_width, value_width);
      const size_t kTestValueWidthBits = (kTestValueWidth * CHAR_BIT) - 1;

      const uint64_t kTruncatedTestValue =
          bit::Extract(kTestValue, bit::MaskFromRange(0, kTestValueWidthBits));

      // Setup: Convert the truncated test address into a byte array,
      // based on the endianness
      auto expected_value_bytes = ToByteArray(endian, kTruncatedTestValue);
      std::array<uint8_t, kArraySize> expected_value = { 0 };

      // BIG endian has all of the bytes at the back of the array, and
      // thus, we need to find the start of the valid data in the array,
      // then copy the bytes in.
      if (endian == std::endian::big)
      {
        // Copy value_bytes into expected_value
        std::copy_n(&expected_value_bytes.end()[-register_width],
                    register_width, expected_value.begin());
      }
      else
      {
        // Copy value_bytes into expected_value
        std::copy_n(expected_value_bytes.begin(), register_width,
                    expected_value.begin());
      }

      // This test is exhaustive in order to prove as much as possible that
      // the memory map works for all supported value formats.
      SUBCASE("Reading Integers: " + test_iteration_counter)
      {
        INFO("Read Int Bytes   = " << value_width);
        mock_map.SetReadValue(expected_value);
        uint64_t read_int = 0;

        // Exercise
        switch (value_width)
        {
          case sizeof(uint8_t):
            read_int = static_cast<uint8_t>(mock_map[test_register]);
            break;
          case sizeof(uint16_t):
            read_int = static_cast<uint16_t>(mock_map[test_register]);
            break;
          case sizeof(uint32_t):
            read_int = static_cast<uint32_t>(mock_map[test_register]);
            break;
          case sizeof(uint64_t):
            read_int = static_cast<uint64_t>(mock_map[test_register]);
            break;
        }

        // Verify
        INFO("test value = 0x" << std::hex << kTruncatedTestValue
                               << " :: read_int = 0x" << read_int);
        CHECK(kTruncatedTestValue == read_int);
        CHECK(test_register.width == mock_map.read_value_span.size());

        CHECK(expected_address == mock_map.read_address);
        CHECK(address_width == mock_map.read_address_span.size());
      }

      // This test is exhaustive in order to prove as much as possible that
      // the memory map works for all supported value formats.
      SUBCASE("Writing Integers:" + test_iteration_counter)
      {
        INFO("Write Int Bytes  = " << value_width);
        // Exercise
        switch (value_width)
        {
          case sizeof(uint8_t):
            mock_map[test_register] = static_cast<uint8_t>(kTestValue);
            break;
          case sizeof(uint16_t):
            mock_map[test_register] = static_cast<uint16_t>(kTestValue);
            break;
          case sizeof(uint32_t):
            mock_map[test_register] = static_cast<uint32_t>(kTestValue);
            break;
          case sizeof(uint64_t):
            mock_map[test_register] = static_cast<uint64_t>(kTestValue);
            break;
        }

        // Verify
        CHECK(expected_value == mock_map.write_value);
        CHECK(test_register.width == mock_map.write_value_span.size());

        CHECK(address_width == mock_map.write_address_span.size());
        CHECK(expected_address == mock_map.write_address);
      }
    }

    SUBCASE("rvalue array assignment: " + test_iteration_counter)
    {
      for (int array_index = 0; array_index < 12; array_index++)
      {
        // Exercise
        switch (array_index)
        {
          case 0:
            mock_map[test_register] = std::array<uint8_t, 1>({ 1 });
            CHECK(std::array<uint8_t, kArraySize>({ 1 }) ==
                  mock_map.write_value);
            CHECK(1 == mock_map.write_value_span.size());
            break;
          case 1:
            mock_map[test_register] = std::array<uint8_t, 2>({ 1, 2 });

            CHECK(std::array<uint8_t, kArraySize>({ 1, 2 }) ==
                  mock_map.write_value);
            CHECK(2 == mock_map.write_value_span.size());
            break;
          case 2:
            mock_map[test_register] = std::array<uint8_t, 3>({ 1, 2, 3 });

            CHECK(std::array<uint8_t, kArraySize>({ 1, 2, 3 }) ==
                  mock_map.write_value);
            CHECK(3 == mock_map.write_value_span.size());
            break;
          case 3:
            mock_map[test_register] = std::array<uint8_t, 4>({ 1, 2, 3, 4 });

            CHECK(std::array<uint8_t, kArraySize>({ 1, 2, 3, 4 }) ==
                  mock_map.write_value);
            CHECK(4 == mock_map.write_value_span.size());
            break;
          case 4:
            mock_map[test_register] = std::array<uint8_t, 5>({ 1, 2, 3, 4, 5 });

            CHECK(std::array<uint8_t, kArraySize>({ 1, 2, 3, 4, 5 }) ==
                  mock_map.write_value);
            CHECK(5 == mock_map.write_value_span.size());
            break;
          case 5:
            mock_map[test_register] =
                std::array<uint8_t, 6>({ 1, 2, 3, 4, 5, 6 });

            CHECK(std::array<uint8_t, kArraySize>({ 1, 2, 3, 4, 5, 6 }) ==
                  mock_map.write_value);
            CHECK(6 == mock_map.write_value_span.size());
            break;
          case 6:
            mock_map[test_register] =
                std::array<uint8_t, 7>({ 1, 2, 3, 4, 5, 6, 7 });

            CHECK(std::array<uint8_t, kArraySize>({ 1, 2, 3, 4, 5, 6, 7 }) ==
                  mock_map.write_value);
            CHECK(7 == mock_map.write_value_span.size());
            break;
          case 7:
            mock_map[test_register] =
                std::array<uint8_t, 8>({ 1, 2, 3, 4, 5, 6, 7, 8 });

            CHECK(std::array<uint8_t, kArraySize>({ 1, 2, 3, 4, 5, 6, 7, 8 }) ==
                  mock_map.write_value);
            CHECK(8 == mock_map.write_value_span.size());
            break;
          case 8:
            mock_map[test_register] =
                std::array<uint8_t, 9>({ 1, 2, 3, 4, 5, 6, 7, 8, 9 });

            CHECK(std::array<uint8_t, kArraySize>(
                      { 1, 2, 3, 4, 5, 6, 7, 8, 9 }) == mock_map.write_value);
            CHECK(9 == mock_map.write_value_span.size());
            break;
          case 9:
            mock_map[test_register] =
                std::array<uint8_t, 10>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });

            CHECK(std::array<uint8_t, kArraySize>(
                      { { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } }) ==
                  mock_map.write_value);
            CHECK(10 == mock_map.write_value_span.size());
            break;
          case 10:
            mock_map[test_register] =
                std::array<uint8_t, 11>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });

            CHECK(std::array<uint8_t, kArraySize>(
                      { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }) ==
                  mock_map.write_value);
            CHECK(11 == mock_map.write_value_span.size());
            break;
          case 11:
            mock_map[test_register] = std::array<uint8_t, 12>(
                { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 });

            CHECK(std::array<uint8_t, kArraySize>(
                      { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }) ==
                  mock_map.write_value);
            CHECK(12 == mock_map.write_value_span.size());
            break;
        }

        // Verify
        CHECK(expected_address == mock_map.write_address);
        CHECK(address_width == mock_map.write_address_span.size());
      }
    }

    SUBCASE("lvalue array assignment: " + test_iteration_counter)
    {
      for (int array_index = 0; array_index < 12; array_index++)
      {
        auto lvalue_array0      = std::array<uint8_t, 1>({ 1 });
        auto lvalue_array0_full = std::array<uint8_t, kArraySize>({ 1 });

        auto lvalue_array1      = std::array<uint8_t, 2>({ 1, 2 });
        auto lvalue_array1_full = std::array<uint8_t, kArraySize>({ 1, 2 });

        auto lvalue_array2      = std::array<uint8_t, 3>({ 1, 2, 3 });
        auto lvalue_array2_full = std::array<uint8_t, kArraySize>({ 1, 2, 3 });

        auto lvalue_array3 = std::array<uint8_t, 4>({ 1, 2, 3, 4 });
        auto lvalue_array3_full =
            std::array<uint8_t, kArraySize>({ 1, 2, 3, 4 });

        auto lvalue_array4 = std::array<uint8_t, 5>({ 1, 2, 3, 4, 5 });
        auto lvalue_array4_full =
            std::array<uint8_t, kArraySize>({ 1, 2, 3, 4, 5 });

        auto lvalue_array5 = std::array<uint8_t, 6>({ 1, 2, 3, 4, 5, 6 });
        auto lvalue_array5_full =
            std::array<uint8_t, kArraySize>({ 1, 2, 3, 4, 5, 6 });

        auto lvalue_array6 = std::array<uint8_t, 7>({ 1, 2, 3, 4, 5, 6, 7 });
        auto lvalue_array6_full =
            std::array<uint8_t, kArraySize>({ 1, 2, 3, 4, 5, 6, 7 });

        auto lvalue_array7 = std::array<uint8_t, 8>({ 1, 2, 3, 4, 5, 6, 7, 8 });
        auto lvalue_array7_full =
            std::array<uint8_t, kArraySize>({ 1, 2, 3, 4, 5, 6, 7, 8 });

        auto lvalue_array8 =
            std::array<uint8_t, 9>({ 1, 2, 3, 4, 5, 6, 7, 8, 9 });
        auto lvalue_array8_full =
            std::array<uint8_t, kArraySize>({ 1, 2, 3, 4, 5, 6, 7, 8, 9 });

        auto lvalue_array9 =
            std::array<uint8_t, 10>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });
        auto lvalue_array9_full =
            std::array<uint8_t, kArraySize>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 });

        auto lvalue_array10 =
            std::array<uint8_t, 11>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });
        auto lvalue_array10_full = std::array<uint8_t, kArraySize>(
            { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 });

        auto lvalue_array11 =
            std::array<uint8_t, 12>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 });
        auto lvalue_array11_full = std::array<uint8_t, kArraySize>(
            { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 });

        // Exercise
        switch (array_index)
        {
          case 0:
            mock_map[test_register] = lvalue_array0;
            CHECK(lvalue_array0_full == mock_map.write_value);
            CHECK(1 == mock_map.write_value_span.size());
            break;
          case 1:
            mock_map[test_register] = lvalue_array1;
            CHECK(lvalue_array1_full == mock_map.write_value);
            CHECK(2 == mock_map.write_value_span.size());
            break;
          case 2:
            mock_map[test_register] = lvalue_array2;
            CHECK(lvalue_array2_full == mock_map.write_value);
            CHECK(3 == mock_map.write_value_span.size());
            break;
          case 3:
            mock_map[test_register] = lvalue_array3;
            CHECK(lvalue_array3_full == mock_map.write_value);
            CHECK(4 == mock_map.write_value_span.size());
            break;
          case 4:
            mock_map[test_register] = lvalue_array4;
            CHECK(lvalue_array4_full == mock_map.write_value);
            CHECK(5 == mock_map.write_value_span.size());
            break;
          case 5:
            mock_map[test_register] = lvalue_array5;
            CHECK(lvalue_array5_full == mock_map.write_value);
            CHECK(6 == mock_map.write_value_span.size());
            break;
          case 6:
            mock_map[test_register] = lvalue_array6;
            CHECK(lvalue_array6_full == mock_map.write_value);
            CHECK(7 == mock_map.write_value_span.size());
            break;
          case 7:
            mock_map[test_register] = lvalue_array7;
            CHECK(lvalue_array7_full == mock_map.write_value);
            CHECK(8 == mock_map.write_value_span.size());
            break;
          case 8:
            mock_map[test_register] = lvalue_array8;
            CHECK(lvalue_array8_full == mock_map.write_value);
            CHECK(9 == mock_map.write_value_span.size());
            break;
          case 9:
            mock_map[test_register] = lvalue_array9;
            CHECK(lvalue_array9_full == mock_map.write_value);
            CHECK(10 == mock_map.write_value_span.size());
            break;
          case 10:
            mock_map[test_register] = lvalue_array10;
            CHECK(lvalue_array10_full == mock_map.write_value);
            CHECK(11 == mock_map.write_value_span.size());
            break;
          case 11:
            mock_map[test_register] = lvalue_array11;
            CHECK(lvalue_array11_full == mock_map.write_value);
            CHECK(12 == mock_map.write_value_span.size());
            break;
        }

        // Verify
        CHECK(expected_address == mock_map.write_address);
        CHECK(address_width == mock_map.write_address_span.size());
      }
    }

    SUBCASE("Read Byte Array: " + test_iteration_counter)
    {
      for (int array_index = 1; array_index < 12; array_index++)
      {
        // Setup
        std::array<uint8_t, kArraySize> set_read_value(
            { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 });

        mock_map.SetReadValue(set_read_value);

        // Exercise
        switch (array_index)
        {
          case 1:
          {
            std::array<uint8_t, 1> read_bytes = mock_map[test_register];
            CHECK(std::array<uint8_t, 1>({ 1 }) == read_bytes);
            CHECK(1 == mock_map.read_value_span.size());
            break;
          }
          case 2:
          {
            std::array<uint8_t, 2> read_bytes = mock_map[test_register];
            CHECK(std::array<uint8_t, 2>({ 1, 2 }) == read_bytes);
            CHECK(2 == mock_map.read_value_span.size());
            break;
          }
          case 3:
          {
            std::array<uint8_t, 3> read_bytes = mock_map[test_register];
            CHECK(std::array<uint8_t, 3>({ 1, 2, 3 }) == read_bytes);
            CHECK(3 == mock_map.read_value_span.size());
            break;
          }
          case 4:
          {
            std::array<uint8_t, 4> read_bytes = mock_map[test_register];
            CHECK(std::array<uint8_t, 4>({ 1, 2, 3, 4 }) == read_bytes);
            CHECK(4 == mock_map.read_value_span.size());
            break;
          }
          case 5:
          {
            std::array<uint8_t, 5> read_bytes = mock_map[test_register];
            CHECK(std::array<uint8_t, 5>({ 1, 2, 3, 4, 5 }) == read_bytes);
            CHECK(5 == mock_map.read_value_span.size());
            break;
          }
          case 6:
          {
            std::array<uint8_t, 6> read_bytes = mock_map[test_register];
            CHECK(std::array<uint8_t, 6>({ 1, 2, 3, 4, 5, 6 }) == read_bytes);
            CHECK(6 == mock_map.read_value_span.size());
            break;
          }
          case 7:
          {
            std::array<uint8_t, 7> read_bytes = mock_map[test_register];
            CHECK(std::array<uint8_t, 7>({ 1, 2, 3, 4, 5, 6, 7 }) ==
                  read_bytes);
            CHECK(7 == mock_map.read_value_span.size());
            break;
          }
          case 8:
          {
            std::array<uint8_t, 8> read_bytes = mock_map[test_register];
            CHECK(std::array<uint8_t, 8>({ 1, 2, 3, 4, 5, 6, 7, 8 }) ==
                  read_bytes);
            CHECK(8 == mock_map.read_value_span.size());
            break;
          }
          case 9:
          {
            std::array<uint8_t, 9> read_bytes = mock_map[test_register];
            CHECK(std::array<uint8_t, 9>({ 1, 2, 3, 4, 5, 6, 7, 8, 9 }) ==
                  read_bytes);
            CHECK(9 == mock_map.read_value_span.size());
            break;
          }
          case 10:
          {
            std::array<uint8_t, 10> read_bytes = mock_map[test_register];
            CHECK(std::array<uint8_t, 10>({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }) ==
                  read_bytes);
            CHECK(10 == mock_map.read_value_span.size());
            break;
          }
          case 11:
          {
            std::array<uint8_t, 11> read_bytes = mock_map[test_register];
            CHECK(std::array<uint8_t, 11>(
                      { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 }) == read_bytes);
            CHECK(11 == mock_map.read_value_span.size());
            break;
          }
          case 12:
          {
            std::array<uint8_t, 12> read_bytes = mock_map[test_register];
            CHECK(std::array<uint8_t, 12>(
                      { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 }) == read_bytes);
            CHECK(12 == mock_map.read_value_span.size());
            break;
          }
        }

        // Verify
        CHECK(expected_address == mock_map.read_address);
        CHECK(address_width == mock_map.read_address_span.size());
      }
    }

    SUBCASE("Read Int Array: " + test_iteration_counter)
    {
      // Setup
      std::array<uint8_t, kArraySize> set_read_value({ 1, 2, 3, 4, 5, 6 });

      mock_map.SetReadValue(set_read_value);

      // Exercise
      if (endian == std::endian::big)
      {
        // Exercise: BIG Endian
        std::array<uint16_t, 3> read_bytes = mock_map[test_register];
        CHECK(std::array<uint16_t, 3>({ 0x0102, 0x0304, 0x0506 }) ==
              read_bytes);
        CHECK(6 == mock_map.read_value_span.size());
      }
      else
      {
        // Exercise: Little Endian
        std::array<uint16_t, 3> read_bytes = mock_map[test_register];
        CHECK(std::array<uint16_t, 3>({ 0x0201, 0x0403, 0x0605 }) ==
              read_bytes);
        CHECK(6 == mock_map.read_value_span.size());
      }
      // Verify
      CHECK(expected_address == mock_map.read_address);
      CHECK(address_width == mock_map.read_address_span.size());
    }

    SUBCASE("Write Int Array: " + test_iteration_counter)
    {
      // Setup
      std::array<uint16_t, 3> kTestArray({ 0x0102, 0x0304, 0x0506 });

      // Exercise
      if (endian == std::endian::big)
      {
        // Exercise: BIG Endian
        mock_map[test_register] = kTestArray;
        CHECK(std::array<uint8_t, kArraySize>({ 1, 2, 3, 4, 5, 6 }) ==
              mock_map.write_value);
        CHECK(6 == mock_map.write_value_span.size());
      }
      else
      {
        // Exercise: Little Endian
        mock_map[test_register] = kTestArray;
        CHECK(std::array<uint8_t, kArraySize>({ 2, 1, 4, 3, 6, 5 }) ==
              mock_map.write_value);
        CHECK(6 == mock_map.write_value_span.size());
      }

      // Verify
      CHECK(expected_address == mock_map.write_address);
      CHECK(address_width == mock_map.write_address_span.size());
    }
  }
}
}  // namespace sjsu

TYPE_TO_STRING(sjsu::DeviceMemoryMap::Register<sjsu::spec1>);
TYPE_TO_STRING(sjsu::DeviceMemoryMap::Register<sjsu::spec2>);
TYPE_TO_STRING(sjsu::DeviceMemoryMap::Register<sjsu::spec3>);
TYPE_TO_STRING(sjsu::DeviceMemoryMap::Register<sjsu::spec4>);
TYPE_TO_STRING(sjsu::DeviceMemoryMap::Register<sjsu::spec5>);
TYPE_TO_STRING(sjsu::DeviceMemoryMap::Register<sjsu::spec6>);
TYPE_TO_STRING(sjsu::DeviceMemoryMap::Register<sjsu::spec7>);
TYPE_TO_STRING(sjsu::DeviceMemoryMap::Register<sjsu::spec8>);

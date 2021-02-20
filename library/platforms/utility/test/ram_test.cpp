#include <cstdint>
#include <cstring>

#include "testing/testing_frameworks.hpp"
#include "platforms/utility/ram.hpp"

namespace sjsu
{
TEST_CASE("Testing Ram Initialization")
{
  SECTION(".data")
  {
    // Setup
    memset(data_section_table[0].rom_location, 0, data_section_table[0].length);

    // Exercise
    sjsu::InitializeDataSection();

    // Verify
    CHECK(memcmp(data_section_table[0].ram_location,
                 data_section_table[0].rom_location,
                 data_section_table[0].length) == 0);
  }

  SECTION(".bss")
  {
    // Setup
    // Setup: Fill the expected_blank_bss_section with all zeros.
    std::array<uint32_t, 512> expected_blank_bss_section;

    memset(expected_blank_bss_section.data(), 0,
           expected_blank_bss_section.size());

    // Setup: Fill the bss_section to be cleared with an arbitrary value. This
    //        value should be cleared to zero after calling
    //        `InitializeBssSection()`
    memset(bss_section_table[0].ram_location, 0xAA,
           bss_section_table[0].length);

    // Exercise
    sjsu::InitializeBssSection();

    // Verify
    CHECK(memcmp(expected_blank_bss_section.data(),
                 bss_section_table[0].ram_location,
                 bss_section_table[0].length) == 0);
  }
}
}  // namespace sjsu

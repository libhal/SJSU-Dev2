#include <cstdint>
#include <cstring>

#include "L4_Testing/testing_frameworks.hpp"
#include "L0_Platform/ram.hpp"

namespace
{
struct DataSection_t
{
  int32_t a;
  uint8_t b;
  double d;
  uint16_t s;
};

DataSection_t rom = {
  .a = 15,
  .b = 'C',
  .d = 5.0,
  .s = 12'346U,
};

DataSection_t ram;

std::array<uint32_t, 128> bss_section;
}  // namespace

DataSectionTable_t data_section_table[] = {
  DataSectionTable_t{
      .rom_location = reinterpret_cast<uint32_t *>(&rom),
      .ram_location = reinterpret_cast<uint32_t *>(&ram),
      .length       = sizeof(rom),
  },
};

// NOTE: Not used, but must be defined
DataSectionTable_t data_section_table_end;

BssSectionTable_t bss_section_table[] = {
  BssSectionTable_t{
      .ram_location = bss_section.data(),
      .length       = bss_section.size(),
  },
};

// NOTE: Not used, but must be defined
BssSectionTable_t bss_section_table_end;

namespace sjsu
{
TEST_CASE("Testing Ram Initialization", "[ram-init]")
{
  SECTION(".data")
  {
    // Setup
    testing::ClearStructure(&ram);

    // Exercise
    sjsu::InitializeDataSection();

    // Verify
    CHECK(memcmp(&rom, &ram, sizeof(rom)) == 0);
  }

  SECTION(".bss")
  {
    // Setup
    // Setup: Fill the expected_blank_bss_section with all zeros.
    std::array<uint32_t, bss_section.size()> expected_blank_bss_section;
    memset(expected_blank_bss_section.data(), 0,
           expected_blank_bss_section.size());
    // Setup: Fill the bss_section to be cleared with an arbitary value. This
    //        value should be cleared to zero after calling
    //        `InitializeBssSection()`
    memset(bss_section.data(), 0xA5A5A5A5, bss_section.size());

    // Exercise
    sjsu::InitializeBssSection();

    // Verify
    CHECK(memcmp(expected_blank_bss_section.data(), bss_section.data(),
                 bss_section.size()) == 0);
  }
}
}  // namespace sjsu

#pragma once

#include <cstddef>
#include <cstdint>
#include "utility/macros.hpp"

// .data Section Table Information
SJ2_PACKED(struct)
DataSectionTable_t
{
  uint32_t * rom_location;
  uint32_t * ram_location;
  uint32_t length;
};

extern DataSectionTable_t data_section_table[];
extern DataSectionTable_t data_section_table_end;

// .bss Section Table Information
SJ2_PACKED(struct)
BssSectionTable_t
{
  uint32_t * ram_location;
  uint32_t length;
};

extern BssSectionTable_t bss_section_table[];
extern BssSectionTable_t bss_section_table_end;

// The address of this array starts at the beginning of RAM2 which is used
// exclusively for heap.
#if defined(HOST_TEST)
inline uint8_t heap;
inline uint8_t heap_end;
#else
extern uint8_t heap;
extern uint8_t heap_end;
#endif  // defined(HOST_TEST)
inline uint8_t * heap_position = &heap;

namespace sjsu
{
/// Copies the defined variabes within the .data section in ROM into RAM
inline void InitializeDataSection()
{
  uint32_t * rom_location = data_section_table[0].rom_location;
  uint32_t * ram_location = data_section_table[0].ram_location;
  uint32_t length         = data_section_table[0].length / sizeof(uint32_t);
  for (size_t j = 0; j < length; j++)
  {
    ram_location[j] = rom_location[j];
  }
}
/// Initializes the .bss section of RAM. The STD C libraries assume that BSS is
/// set to zero and will fault otherwise.
inline void InitializeBssSection()
{
  uint32_t * ram_location = bss_section_table[0].ram_location;
  uint32_t length         = bss_section_table[0].length / sizeof(uint32_t);
  for (size_t j = 0; j < length; j++)
  {
    ram_location[j] = 0;
  }
}
}  // namespace sjsu

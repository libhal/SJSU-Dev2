#pragma once

#include <cstddef>
#include <cstdint>
#include "utility/macros.hpp"
// ========================
// Memory Map information
// ========================
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
// Functions to carry out the initialization of RW and BSS data sections.
inline void InitializeDataSection()
{
  for (int i = 0; &data_section_table[i] < &data_section_table_end; i++)
  {
    uint32_t * rom_location = data_section_table[i].rom_location;
    uint32_t * ram_location = data_section_table[i].ram_location;
    uint32_t length         = data_section_table[i].length;
    for (size_t j = 0; j < length; j++)
    {
      ram_location[j] = rom_location[j];
    }
  }
}
// Functions to initialization BSS data sections. This is important because
// the std c libs assume that BSS is set to zero.
inline void InitializeBssSection()
{
  for (int i = 0; &bss_section_table[i] < &bss_section_table_end; i++)
  {
    uint32_t * ram_location = bss_section_table[i].ram_location;
    uint32_t length         = bss_section_table[i].length;
    for (size_t j = 0; j < length; j++)
    {
      ram_location[j] = 0;
    }
  }
}
}  // namespace sjsu

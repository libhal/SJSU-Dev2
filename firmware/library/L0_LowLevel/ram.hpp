// TODO(kammce): rename this file to ram.hpp
#pragma once
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

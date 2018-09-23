#pragma once

#include <cstdint>

namespace utility
{
uint32_t Random(uint32_t min, uint32_t max);
void SetRandomSeed(uint32_t = 0xFFFFFFFF);
uint32_t BitExtract(uint32_t target, uint8_t start_bit, uint8_t width);
uint32_t BitPlace(uint32_t target, uint8_t start_bit, uint8_t new_bits,
                  uint8_t width);
uint32_t BitPlace(uint32_t target, uint8_t start_bit, bool new_bit,
                  uint8_t width);
uint32_t BitSet(uint32_t target, uint8_t bit_position);
uint32_t BitClear(uint32_t target, uint8_t bit_position);
uint32_t BitRead(uint32_t target, uint8_t bit_position);
uint32_t Min(uint32_t compare_1, uint32_t compare_2);
uint32_t Max(uint32_t compare_1, uint32_t compare_2);
uint32_t Map(uint32_t value, uint32_t current_min, uint32_t current_max,
             uint32_t new_min, uint32_t new_max);
uint32_t Constrain(uint32_t value, uint32_t min, uint32_t max);
}  // namespace utility

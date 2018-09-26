#pragma once

#include <cstdint>

namespace utility
{
int32_t Random(int32_t min, int32_t max);
void SetRandomSeed(uint32_t = 0xFFFFFFFF);
uint32_t BitExtract(uint32_t target, uint8_t start_bit, uint8_t width);
uint32_t BitPlace(uint32_t target, uint8_t start_bit, uint32_t new_bits,
                  uint8_t width);
uint32_t BitSet(uint32_t target, uint8_t bit_position, uint8_t bit_width = 1);
uint32_t BitClear(uint32_t target, uint8_t bit_position, uint8_t bitwidth = 1);
bool BitRead(uint32_t target, uint8_t bit_position);
template <typename T>
T Min(T compare_1, T compare_2)
{
    return compare_1 <= compare_2 ? compare_1 : compare_2;
};
template <typename T>
T Max(T compare_1, T compare_2)
{
    return compare_1 >= compare_2 ? compare_1 : compare_2;
};
template <typename T>
T Map(T value, T current_min, T current_max, T new_min, T new_max)
{
    double map_value       = value;
    double map_current_min = current_min;
    double map_current_max = current_max;
    double map_new_min     = new_min;
    double map_new_max     = new_max;
    double mapped          = (map_value - map_current_min) *
                        (map_new_max - map_new_min) /
                        (map_current_max - map_current_min) +
                    map_new_min;
    return static_cast<T>(mapped);
};
template <typename T>
bool FloatCompare(T between, T value)
{
	return (between - 0.1 <= value && value + 0.1 >= between) ? true : false;
}
}

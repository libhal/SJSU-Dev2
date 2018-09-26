#include <cstdlib>

#include "utility.hpp"

int32_t utility::Random(int32_t min, int32_t max)
{
    return rand() % (max - min + 1) + min;
}
void utility::SetRandomSeed(uint32_t seed)
{
    srand(seed);
}
uint32_t utility::BitExtract(uint32_t target, uint8_t start_bit, uint8_t width)
{
    uint32_t mask = 0;
    for (int i = 0; i < width; i++)
    {
        mask = (mask << 1) + 1;
    }
    uint8_t true_start_bit = static_cast<uint8_t>(start_bit - (width - 1));
    mask <<= true_start_bit;
    return (target & mask) >> true_start_bit;
}
uint32_t utility::BitPlace(uint32_t target, uint8_t start_bit,
                           uint32_t new_bits, uint8_t width)
{
    uint32_t mask = 0xFFFFFFFF >> (32 - width);
    target &= ~(mask << (start_bit - (width - 1)));
    return target | ((new_bits & mask) << (start_bit - (width - 1)));
}

uint32_t utility::BitSet(uint32_t target, uint8_t bit_position,
                         uint8_t bit_width)
{
    uint32_t mask = 0xFFFFFFFF >> (32 - bit_width);
    mask <<= (bit_position - (bit_width - 1));
    return target | mask;
}
uint32_t utility::BitClear(uint32_t target, uint8_t bit_position,
                           uint8_t bit_width)
{
    uint32_t mask = 0xFFFFFFFF >> (32 - bit_width);
    mask <<= (bit_position - (bit_width - 1));
    return target & ~mask;
}
bool utility::BitRead(uint32_t target, uint8_t bit_position)
{
    uint32_t mask       = 1 << bit_position;
    uint32_t masked_var = target & mask;
    return masked_var >> bit_position;
}

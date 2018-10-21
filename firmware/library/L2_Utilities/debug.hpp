#pragma once

#include <cstdint>
#include <cstdlib>

namespace debug
{
void Hexdump(void * address, uint32_t length);
void PrintTrace();
}  // namespace debug

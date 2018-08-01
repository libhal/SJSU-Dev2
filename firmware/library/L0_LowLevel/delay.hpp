#pragma once

#include <cstdint>

extern volatile uint64_t milliseconds;

uint64_t Milliseconds();

void Delay(uint32_t delay);

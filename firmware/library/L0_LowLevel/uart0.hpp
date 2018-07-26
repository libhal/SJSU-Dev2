// Minimum required implementations of uart0 to make print statements work
#pragma once

#include "LPC40xx.h"

namespace uart0
{

void Init(uint32_t baud_rate);
char GetChar(char notused);
char PutChar(char out);
void Puts(const char c_string[]);

}  // namespace uart0

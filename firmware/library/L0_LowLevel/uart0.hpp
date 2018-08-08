// Minimum required implementations of uart0 to make print statements work
#pragma once

#include "LPC40xx.h"

namespace uart0
{

void Init(uint32_t baud_rate);
int GetChar();
int GetChar(int notused);
int PutChar(int out);
void Puts(const char c_string[]);

}  // namespace uart0

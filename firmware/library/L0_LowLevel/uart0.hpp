// Minimum required implementations of uart0 to make print statements work
#pragma once

#include "LPC40xx.h"
#include "L1_Drivers/gpio.hpp"

namespace uart0
{

extern PinConfigureInterface * rx;
extern PinConfigureInterface * tx;

extern LPC_UART_TypeDef * uart0_register;
extern LPC_SC_TypeDef * sc;

void Init(uint32_t baud_rate);
int GetChar();
char GetChar(uint32_t timeout = 0x7FFFFFFF);
int PutChar(int out);
char PutChar(char out);
void Puts(const char c_string[]);

}  // namespace uart0

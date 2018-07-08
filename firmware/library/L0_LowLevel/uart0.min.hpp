// Minimum required implementations of uart0 to make print statements work
#pragma once

#include "LPC40xx.h"

void uart0_init(uint32_t baud_rate);
char uart0_getchar(char notused);
char uart0_putchar(char out);
void uart0_puts(const char c_string[]);

#ifndef UART0_MIN_H
#define UART0_MIN_H

#include "LPC40xx.h"

void uart0_init(unsigned int baud_rate);
char uart0_getchar(char notused);
char uart0_putchar(char out);
void uart0_puts(const char * c_string);

#endif

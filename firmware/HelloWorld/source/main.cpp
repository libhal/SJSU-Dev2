#include <cstdio>
#include <cstdint>
#include "L0-LowLevel/LPC40xx.h"
#include "L0-LowLevel/uart0.min.hpp"

uint32_t cycles = 500'000;
int main(void)
{
    uart0_init(38400);
    uart0_puts("Booted!\n");
    uart0_puts("Initializing LEDs...\n");
    LPC_IOCON->P1_1 &= ~(0b111);
    LPC_IOCON->P1_8 &= ~(0b111);
    LPC_GPIO1->DIR |=  (1 << 1);
    LPC_GPIO1->PIN &= ~(1 << 1);
    LPC_GPIO1->DIR |=  (1 << 8);
    LPC_GPIO1->PIN |=  (1 << 8);
    uart0_puts("LEDs Initialized...\n");
    uart0_puts("Toggling LEDs...\n");
    while(1)
    {
        for(uint32_t i = 0; i < cycles; i++);
        LPC_GPIO1->PIN ^= 0b0001'0000'0010;
    }
    
    return 0;
}
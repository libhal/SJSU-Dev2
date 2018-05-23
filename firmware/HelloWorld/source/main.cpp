#include <stdio.h>
#include <stdint.h>
#include "L0-LowLevel/LPC40xx.h"
#include "L0-LowLevel/uart0.min.hpp"

int main(void)
{
    LPC_SC->PCONP |= (1 << 15);
    // LPC_IOCON->P1_15 &= (0b111 << 0);

    LPC_GPIO1->DIR |= 1 << 15;
    LPC_GPIO1->CLR |= 1 << 15;

    LPC_GPIO1->DIR |= 1 << 10;
    LPC_GPIO1->CLR |= 1 << 10;
    uart0_puts("Hello, World\n");
    return 0;
}
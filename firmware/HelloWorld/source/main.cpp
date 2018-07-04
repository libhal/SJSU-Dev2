#include <cstdio>
#include <cstdint>
#include <cstdarg>
#include <cstdlib>
#include "L0_LowLevel/LPC40xx.h"
#include "L2_Utilities/debug_print.hpp"

volatile uint32_t cycles = 500'000;

int main(void)
{
    puts(
        "\x1b[42m================================== SJTwo Booted! "
        "==================================\x1b[0m\n");
    DEBUG_PRINT("Initializing LEDs...\n");
    LPC_IOCON->P1_1 &= ~(0b111);
    LPC_IOCON->P1_8 &= ~(0b111);
    LPC_GPIO1->DIR |= (1 << 1);
    LPC_GPIO1->PIN &= ~(1 << 1);
    LPC_GPIO1->DIR |= (1 << 8);
    LPC_GPIO1->PIN |= (1 << 8);
    DEBUG_PRINT("LEDs Initialized...\n");
    fputs("Enter wait cycles for led animation: ", stdout);
    scanf("%lu", &cycles);
    DEBUG_PRINT("Toggling LEDs...\n");

    while (1)
    {
        for (uint32_t i = 0; i < cycles; i++) { continue; }
        LPC_GPIO1->PIN ^= 0b0001'0000'0010;
    }
    return 0;
}

#include <project_config.hpp>

#include <cstdint>
#include <cstdio>

#include "L0_LowLevel/LPC40xx.h"
#include "L2_Utilities/debug_print.hpp"

volatile uint32_t cycles = 500'000;

int main(void)
{
#if SJ2_ENABLE_ANSI_CODES
    // Clears the terminal
    fputs("\e[2J\e[H", stdout);
#endif
    fputs(SJ2_BACKGROUND_GREEN
         "================================== SJTwo Booted! "
         "==================================\n" SJ2_COLOR_RESET, stdout);
    DEBUG_PRINT("Initializing LEDs...");
    LPC_IOCON->P1_1 &= ~(0b111);
    LPC_IOCON->P1_8 &= ~(0b111);
    LPC_GPIO1->DIR |= (1 << 1);
    LPC_GPIO1->PIN &= ~(1 << 1);
    LPC_GPIO1->DIR |= (1 << 8);
    LPC_GPIO1->PIN |= (1 << 8);
    DEBUG_PRINT("LEDs Initialized...");
    fputs("Enter wait cycles for led animation: ", stdout);
    scanf("%lu", &cycles);
    DEBUG_PRINT("Toggling LEDs...");

    while (1)
    {
        for (uint32_t i = 0; i < cycles; i++) { continue; }
        LPC_GPIO1->PIN ^= 0b0001'0000'0010;
    }
    return 0;
}

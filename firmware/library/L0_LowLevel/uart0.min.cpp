#include <cstdint>
#include "LPC40xx.h"
#include "uart0.min.hpp"

void uart0_init(uint32_t baud_rate)
{
    // Adding 0.5 to perform rounding correctly since we do not want
    // 1.9 to round down to 1, but we want it to round-up to 2.
    float baud_rate_float = static_cast<float>(baud_rate);
    const uint32_t divider =
        static_cast<uint32_t>(OSC_CLK / (16.0f * baud_rate_float) + 0.5f);
    const uint16_t dlab_bit          = (1 << 7);
    const uint32_t eight_bit_datalen = 3;

    LPC_SC->PCONP |= (1 << 3);
    LPC_SC->PCLKSEL = 1;

    LPC_IOCON->P0_2 = 1;
    LPC_IOCON->P0_3 = 1;

    LPC_UART0->LCR = dlab_bit;  // Set DLAB bit to access DLM & DLL
    LPC_UART0->DLM = static_cast<uint8_t>(divider >> 8);
    LPC_UART0->DLL = static_cast<uint8_t>(divider >> 0);
    LPC_UART0->LCR = eight_bit_datalen;  // DLAB is reset back to zero
    LPC_UART0->FCR |= (1 << 0);
}

char uart0_getchar(char notused = 0)
{
    (void)(notused);
    while (!(LPC_UART0->LSR & 0x1)) { continue; }
    return LPC_UART0->RBR;
}

char uart0_putchar(char out)
{
    LPC_UART0->THR = out;

    while (!(LPC_UART0->LSR & (0x1 << 6))) { continue; }
    return 1;
}

void uart0_puts(const char * c_string)
{
    for (uint32_t i = 0; c_string[i] != '\0'; i++)
    {
        uart0_putchar(c_string[i]);
    }
}

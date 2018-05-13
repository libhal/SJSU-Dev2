#include <stdint.h>
#include "LPC40xx.h"
#include "uart0.min.h"

void uart0_init(uint32_t baud_rate)
{
    /* Adding 0.5 to perform rounding correctly since we do not want
     * 1.9 to round down to 1, but we want it to round-up to 2.
     */
    float baud_rate_float = (float)(baud_rate);
    const uint32_t divider = (uint32_t)(OSC_CLK / (16.0f * baud_rate_float) + 0.5f);
    const uint16_t dlab_bit = (1 << 7);
    const uint32_t eight_bit_datalen = 3;

    LPC_SC->PCONP |= (1 << 3);
    LPC_SC->PCLKSEL = 1;

    LPC_IOCON->P0_2 = 1;
    LPC_IOCON->P0_3 = 1;

    LPC_UART0->LCR  = dlab_bit;          // Set DLAB bit to access DLM & DLL
    LPC_UART0->DLM  = (uint8_t)(divider >> 8);
    LPC_UART0->DLL  = (uint8_t)(divider >> 0);
    LPC_UART0->LCR  = eight_bit_datalen; // DLAB is reset back to zero
    LPC_UART0->FCR |= (1 << 0);
}

char uart0_getchar(char notused)
{
    (void)(notused);
    while(!(LPC_UART0->LSR & 0x1));
    return LPC_UART0->RBR;
}

char uart0_putchar(char out)
{
    //while(! (LPC_UART0->LSR & (1 << 6)));
    LPC_UART0->THR = out;

    while(!(LPC_UART0->LSR & (0x1 << 6)));

    return 1;
}

void uart0_puts(const char * c_string)
{
    for(uint32_t i = 0; c_string[i] != '\0'; i++)
    {
        uart0_putchar(c_string[i]);
    }
}

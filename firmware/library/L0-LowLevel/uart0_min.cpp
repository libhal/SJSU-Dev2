#include <stdint.h>
#include "LPC40xx.h"

typedef union
{
    LPC_IOCON_TypeDef origin;
    uint32_t matrix[5][32];
} iocon_union_t;

iocon_union_t * const PIN_SELECTOR = (iocon_union_t *)(LPC_IOCON);

void uart0_init(unsigned int baud_rate)
{
    /* Adding 0.5 to perform rounding correctly since we do not want
     * 1.9 to round down to 1, but we want it to round-up to 2.
     */
    const uint16_t divider = (48000000 / (16 * baud_rate) + 0.5);
    const uint8_t dlab_bit = (1 << 7);
    const uint8_t eight_bit_datalen = 3;

    LPC_SC->PCONP |= (1 << 3);
    LPC_SC->PCLKSEL = 1;

    PIN_SELECTOR->matrix[0][2] = 1;
    // LPC_IOCON->P0_2 = 1;
    LPC_IOCON->P0_3 = 1;

    LPC_UART0->LCR  = dlab_bit;          // Set DLAB bit to access DLM & DLL
    LPC_UART0->DLM  = (divider >> 8);
    LPC_UART0->DLL  = (divider >> 0);
    LPC_UART0->LCR  = eight_bit_datalen; // DLAB is reset back to zero
    LPC_UART0->FCR |= (1 << 0);
}

char uart0_getchar(char notused)
{
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
    char* p = (char*) c_string;
    while(*p)
    {
        uart0_putchar(*p);
        p++;
    }
    uart0_putchar('\n');
}

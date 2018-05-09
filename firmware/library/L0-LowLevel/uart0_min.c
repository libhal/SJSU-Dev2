/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

#include "LPC40xx.h"
// #include "sys_config.h"

void uart0_init(unsigned int baud_rate)
{
    /* Adding 0.5 to perform rounding correctly since we do not want
     * 1.9 to round down to 1, but we want it to round-up to 2.
     */
    const uint16_t divider = (sys_get_cpu_clock() / (16 * baud_rate) + 0.5);
    const uint8_t dlab_bit = (1 << 7);
    const uint8_t eight_bit_datalen = 3;

    lpc_pconp(pconp_uart0, true);
    lpc_pclk(pclk_uart0, clkdiv_1);

    LPC_PINCON->PINSEL0 &= ~(0xF << 4); // Clear values
    LPC_PINCON->PINSEL0 |= (0x5 << 4);  // Set values for UART0 Rx/Tx

    LPC_UART0->LCR = dlab_bit;          // Set DLAB bit to access DLM & DLL
    LPC_UART0->DLM = (divider >> 8);
    LPC_UART0->DLL = (divider >> 0);
    LPC_UART0->LCR = eight_bit_datalen; // DLAB is reset back to zero
}

char uart0_getchar(char notused)
{
    while(! BIT(LPC_UART0->LSR).b0);

    return LPC_UART0->RBR;
}

char uart0_putchar(char out)
{
    //while(! (LPC_UART0->LSR & (1 << 6)));
    LPC_UART0->THR = out;

    while(! BIT(LPC_UART0->LSR).b6);

    return 1;
}

void uart0_puts(const char* c_string)
{
    char* p = (char*) c_string;
    while(*p)
    {
        uart0_putchar(*p);
        p++;
    }
    uart0_putchar('\n');
}

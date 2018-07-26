#include <cstdint>

#include "LPC40xx.h"
#include "L0_LowLevel/uart0.hpp"
#include "L2_Utilities/macros.hpp"

namespace uart0
{

void Init(uint32_t baud_rate)
{
    // Adding 0.5 to perform rounding correctly since we do not want
    // 1.9 to round down to 1, but we want it to round-up to 2.
    float baud_rate_float = static_cast<float>(baud_rate);
    uint32_t divider =
        static_cast<uint32_t>(OSC_CLK / (16.0f * baud_rate_float) + 0.5f);
    uint8_t dlab_bit          = (1 << 7);
    uint8_t eight_bit_datalen = 3;

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

char GetChar(char notused = 0)
{
    SJ2_USED(notused);
    while (!(LPC_UART0->LSR & 0x1))
    {
        continue;
    }
    return LPC_UART0->RBR;
}

char PutChar(char out)
{
    LPC_UART0->THR = out;

    while (!(LPC_UART0->LSR & (0x1 << 6)))
    {
        continue;
    }
    return 1;
}

void Puts(const char * c_string)
{
    for (uint32_t i = 0; c_string[i] != '\0'; i++)
    {
        PutChar(c_string[i]);
    }
}

}  // namespace uart0

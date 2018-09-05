#include <cstdint>

#include "config.hpp"
#include "LPC40xx.h"
#include "L0_LowLevel/delay.hpp"
#include "L0_LowLevel/uart0.hpp"
#include "L1_Drivers/gpio.hpp"
#include "L2_Utilities/macros.hpp"

namespace uart0
{

PinConfigure rx_pin(0, 3);
PinConfigureInterface * rx = &rx_pin;

PinConfigure tx_pin(0, 2);
PinConfigureInterface * tx = &tx_pin;

LPC_UART_TypeDef * uart0_register = uart0_register;
LPC_SC_TypeDef * sc = LPC_SC;

void Init(uint32_t baud_rate)
{
    // Adding 0.5 to perform rounding correctly since we do not want
    // 1.9 to round down to 1, but we want it to round-up to 2.
    float baud_rate_float = static_cast<float>(baud_rate);
    uint32_t divider      = static_cast<uint32_t>(
        config::kSystemClockRate / (16.0f * baud_rate_float) + 0.5f);
    uint8_t dlab_bit          = (1 << 7);
    uint8_t eight_bit_datalen = 3;

    sc->PCONP |= (1 << 3);

    constexpr uint8_t kUartFunction = 1;

    tx->SetPinMode(PinConfigureInterface::PinMode::kPullUp);
    rx->SetPinMode(PinConfigureInterface::PinMode::kPullUp);
    tx->SetPinFunction(kUartFunction);
    rx->SetPinFunction(kUartFunction);

    uart0_register->LCR = dlab_bit;  // Set DLAB bit to access DLM & DLL
    uart0_register->DLM = static_cast<uint8_t>(divider >> 8);
    uart0_register->DLL = static_cast<uint8_t>(divider >> 0);
    uart0_register->LCR = eight_bit_datalen;  // DLAB is reset back to zero
    uart0_register->FCR |= (1 << 0);  // Enable FIFO
}

int GetChar()
{
    return GetChar(0xFFFFFFFF);
}

char GetChar(uint32_t timeout)
{
    uint64_t timeout_time = Milliseconds() + timeout;
    uint64_t current_time = Milliseconds();
    char result;
    while (!(uart0_register->LSR & 0x1) && current_time < timeout_time)
    {
        current_time = Milliseconds();
    }
    if (!(uart0_register->LSR & 0x1) && current_time >= timeout_time)
    {
        result = '\xFF';
    }
    else
    {
        result = uart0_register->RBR;
    }
    return result;
}

int PutChar(int out)
{
    return PutChar(static_cast<char>(out));
}

char PutChar(char out)
{
    uart0_register->THR = static_cast<uint8_t>(out);

    while (!(uart0_register->LSR & (0x1 << 6)))
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

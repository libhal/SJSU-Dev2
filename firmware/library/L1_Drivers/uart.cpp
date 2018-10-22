#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin.hpp"
#include "L1_Drivers/uart.hpp"

LPC_UART_TypeDef * Uart::uart_base_reg[4] = {
  [0] = LPC_UART0,
  [1] = LPC_UART2,
  [2] = LPC_UART3,
  [3] = reinterpret_cast<LPC_UART_TypeDef *>(LPC_UART4)
};

Pin Uart::pairs[4][2] = { { Pin(0, 0), Pin(0, 1) },
                          { Pin(2, 8), Pin(2, 9) },
                          { Pin(4, 28), Pin(4, 29) },
                          { Pin(1, 29), Pin(2, 9) } };

LPC_SC_TypeDef * Uart::sysclock_register = LPC_SC;

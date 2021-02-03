#pragma once

#include "platforms/targets/lpc17xx/LPC17xx.h"
#include "peripherals/lpc17xx/pin.hpp"
#include "peripherals/lpc40xx/system_controller.hpp"
#include "peripherals/lpc40xx/uart.hpp"

namespace sjsu::lpc17xx
{
// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Uart;

template <int port>
inline Uart & GetUart()
{
  if constexpr (port == 0)
  {
    static lpc17xx::Pin & uart0_tx = lpc17xx::GetPin<0, 2>();
    static lpc17xx::Pin & uart0_rx = lpc17xx::GetPin<0, 3>();
    /// Definition for uart port 0 for lpc40xx.
    static const lpc40xx::Uart::Port_t kUart0 = {
      .registers   = reinterpret_cast<lpc40xx::LPC_UART_TypeDef *>(LPC_UART0),
      .power_on_id = lpc40xx::SystemController::Peripherals::kUart0,
      .tx          = uart0_tx,
      .rx          = uart0_rx,
      .tx_function_id = 0b01,
      .rx_function_id = 0b01,
    };

    static Uart uart(kUart0);
    return uart;
  }
  else if constexpr (port == 2)
  {
    static lpc17xx::Pin & uart2_tx = lpc17xx::GetPin<2, 8>();
    static lpc17xx::Pin & uart2_rx = lpc17xx::GetPin<2, 9>();
    /// Definition for uart port 2 for lpc40xx.
    static const lpc40xx::Uart::Port_t kUart2 = {
      .registers   = reinterpret_cast<lpc40xx::LPC_UART_TypeDef *>(LPC_UART2),
      .power_on_id = lpc40xx::SystemController::Peripherals::kUart2,
      .tx          = uart2_tx,
      .rx          = uart2_rx,
      .tx_function_id = 0b010,
      .rx_function_id = 0b010,
    };

    static Uart uart(kUart2);
    return uart;
  }
  else if constexpr (port == 3)
  {
    static lpc17xx::Pin & uart3_tx = lpc17xx::GetPin<4, 28>();
    static lpc17xx::Pin & uart3_rx = lpc17xx::GetPin<4, 29>();
    /// Definition for uart port 3 for lpc40xx.
    static const lpc40xx::Uart::Port_t kUart3 = {
      .registers   = reinterpret_cast<lpc40xx::LPC_UART_TypeDef *>(LPC_UART3),
      .power_on_id = lpc40xx::SystemController::Peripherals::kUart3,
      .tx          = uart3_tx,
      .rx          = uart3_rx,
      .tx_function_id = 0b010,
      .rx_function_id = 0b010,
    };

    static Uart uart(kUart3);
    return uart;
  }
  else
  {
    static_assert(
        InvalidOption<port>,
        SJ2_ERROR_MESSAGE_DECORATOR("Support UART ports for LPC40xx are UART0, "
                                    "UART2, UART3."));
    return GetUart<0>();
  }
}
}  // namespace sjsu::lpc17xx

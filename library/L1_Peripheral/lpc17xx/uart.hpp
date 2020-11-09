#pragma once

#include "L0_Platform/lpc17xx/LPC17xx.h"
#include "L1_Peripheral/lpc17xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "L1_Peripheral/lpc40xx/uart.hpp"

namespace sjsu
{
namespace lpc17xx
{
// The LPC40xx driver is compatible with the lpc17xx peripheral
using ::sjsu::lpc40xx::Uart;
/// Structure used as a namespace for predefined Port_t definitions.
struct UartPort  // NOLINT
{
 private:
  inline static lpc17xx::Pin uart0_tx = lpc17xx::Pin(0, 2);
  inline static lpc17xx::Pin uart0_rx = lpc17xx::Pin(0, 3);

  inline static lpc17xx::Pin uart2_tx = lpc17xx::Pin(2, 8);
  inline static lpc17xx::Pin uart2_rx = lpc17xx::Pin(2, 9);

  inline static lpc17xx::Pin uart3_tx = lpc17xx::Pin(4, 28);
  inline static lpc17xx::Pin uart3_rx = lpc17xx::Pin(4, 29);

 public:
  /// Definition for uart port 0 for lpc40xx.
  inline static const lpc40xx::Uart::Port_t kUart0 = {
    .registers      = reinterpret_cast<lpc40xx::LPC_UART_TypeDef *>(LPC_UART0),
    .power_on_id    = lpc40xx::SystemController::Peripherals::kUart0,
    .tx             = uart0_tx,
    .rx             = uart0_rx,
    .tx_function_id = 0b01,
    .rx_function_id = 0b01,
  };
  /// Definition for uart port 1 for lpc40xx.
  inline static const lpc40xx::Uart::Port_t kUart2 = {
    .registers      = reinterpret_cast<lpc40xx::LPC_UART_TypeDef *>(LPC_UART2),
    .power_on_id    = lpc40xx::SystemController::Peripherals::kUart2,
    .tx             = uart2_tx,
    .rx             = uart2_rx,
    .tx_function_id = 0b010,
    .rx_function_id = 0b010,
  };
  /// Definition for uart port 2 for lpc40xx.
  inline static const lpc40xx::Uart::Port_t kUart3 = {
    .registers      = reinterpret_cast<lpc40xx::LPC_UART_TypeDef *>(LPC_UART3),
    .power_on_id    = lpc40xx::SystemController::Peripherals::kUart3,
    .tx             = uart3_tx,
    .rx             = uart3_rx,
    .tx_function_id = 0b010,
    .rx_function_id = 0b010,
  };
};

}  // namespace lpc17xx
}  // namespace sjsu

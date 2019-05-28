#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/uart.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Uart);

TEST_CASE("Testing lpc40xx Uart", "[lpc40xx-Uart]")
{
  // Simulated local version of LPC_UART2 to verify registers
  LPC_UART_TypeDef local_uart;
  LPC_SC_TypeDef local_sc;

  memset(&local_uart, 0, sizeof(local_uart));
  memset(&local_sc, 0, sizeof(local_sc));

  sjsu::lpc40xx::SystemController::system_controller = &local_sc;

  Mock<sjsu::Pin> mock_tx;
  Fake(Method(mock_tx, SetPinFunction));
  Fake(Method(mock_tx, SetMode));
  Mock<sjsu::Pin> mock_rx;
  Fake(Method(mock_rx, SetPinFunction));
  Fake(Method(mock_rx, SetMode));

  // Set up for UART2
  // Parameters for constructor
  const Uart::Port_t kMockUart2 = {
    .registers      = &local_uart,
    .power_on_id    = sjsu::lpc40xx::SystemController::Peripherals::kUart2,
    .tx             = mock_tx.get(),
    .rx             = mock_rx.get(),
    .tx_function_id = 0b001,
    .rx_function_id = 0b001,
  };
  constexpr uint32_t kBaudRate = 9600;

  Uart uart_test(kMockUart2);
  uart_test.Initialize(kBaudRate);

  SECTION("Initialize")
  {
    constexpr uint32_t kFifo              = 1 << 0;
    constexpr uint32_t kBits8DlabClear    = 3;
    constexpr uint32_t kExpectedUpperByte = 0;
    constexpr uint32_t kExpectedLowerByte = 208;
    constexpr uint32_t kExpectedDivAdd    = 1;
    constexpr uint32_t kExpectedMul       = 2;
    constexpr uint32_t kExpectedFdr = (kExpectedMul << 4) | kExpectedDivAdd;

    CHECK(sjsu::lpc40xx::SystemController().IsPeripheralPoweredUp(
        sjsu::lpc40xx::SystemController::Peripherals::kUart2));

    Verify(Method(mock_tx, SetMode).Using(sjsu::Pin::Mode::kPullUp)).Once();
    Verify(Method(mock_tx, SetPinFunction).Using(kMockUart2.tx_function_id))
        .Once();
    Verify(Method(mock_rx, SetMode).Using(sjsu::Pin::Mode::kPullUp)).Once();
    Verify(Method(mock_tx, SetPinFunction).Using(kMockUart2.rx_function_id))
        .Once();

    CHECK(kExpectedUpperByte == local_uart.DLM);
    CHECK(kExpectedLowerByte == local_uart.DLL);
    CHECK(kExpectedFdr == local_uart.FDR);
    CHECK(kBits8DlabClear == local_uart.LCR);
    CHECK(kFifo == (local_uart.FCR & kFifo));
  }

  sjsu::lpc40xx::SystemController::system_controller = LPC_SC;
}
}  // namespace sjsu::lpc40xx

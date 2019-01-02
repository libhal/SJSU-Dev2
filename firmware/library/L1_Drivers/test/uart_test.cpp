#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/uart.hpp"
#include "L4_Testing/testing_frameworks.hpp"

EMIT_ALL_METHODS(Uart);

TEST_CASE("Testing Uart", "[Uart]")
{
  // Simulated local version of LPC_UART2 to verify registers
  LPC_UART_TypeDef local_uart;
  LPC_SC_TypeDef local_sc;

  memset(&local_uart, 0, sizeof(local_uart));
  memset(&local_sc, 0, sizeof(local_sc));

  Mock<PinInterface> mock_tx;
  Fake(Method(mock_tx, SetPinFunction));
  Fake(Method(mock_tx, SetMode));
  Mock<PinInterface> mock_rx;
  Fake(Method(mock_rx, SetPinFunction));
  Fake(Method(mock_rx, SetMode));

  // Set up for UART2
  Uart::uart[1]  = &local_uart;
  Lpc40xxSystemController::system_controller = &local_sc;

  PinInterface * tx = &mock_tx.get();
  PinInterface * rx = &mock_rx.get();

  // Parameters for constructor
  constexpr uint32_t kBaudRate = 9600;

  Uart uart_test(Uart::Channels::kUart2, tx, rx);
  uart_test.Initialize(kBaudRate);

  SECTION("Initialize")
  {
    constexpr uint32_t kPconpUart2        = 1 << 24;
    constexpr uint32_t kFifo              = 1 << 0;
    constexpr uint32_t kBits8DlabClear    = 3;
    constexpr uint32_t kExpectedUpperByte = 0;
    constexpr uint32_t kExpectedDivAdd    = 1;
    constexpr uint32_t kExpectedMul       = 2;
    constexpr uint32_t kExpectedFdr = (kExpectedMul << 4) | kExpectedDivAdd;

    CHECK(kPconpUart2 == (kPconpUart2 & local_sc.PCONP));

    Verify(Method(mock_tx, SetMode).Using(PinInterface::Mode::kPullUp)).Once();
    Verify(Method(mock_tx, SetPinFunction).Using(2)).Once();
    Verify(Method(mock_rx, SetMode).Using(PinInterface::Mode::kPullUp)).Once();
    Verify(Method(mock_tx, SetPinFunction).Using(2)).Once();

    CHECK(kExpectedUpperByte == local_uart.DLM);
    // CHECK(kExpectedLowerByte == local_uart.DLL);
    CHECK(kExpectedFdr == local_uart.FDR);
    CHECK(kBits8DlabClear == local_uart.LCR);
    CHECK(kFifo == (local_uart.FCR & kFifo));
  }
  Lpc40xxSystemController::system_controller = LPC_SC;
  Uart::uart[1]  = LPC_UART2;
}

#include <cstdint>

#include "config.hpp"
#include "LPC40xx.h"
#include "L0_LowLevel/uart0.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing Uart0", "[uart0]")
{
  using fakeit::Fake;
  using fakeit::Mock;
  using fakeit::Verify;

  LPC_UART_TypeDef local_uart0_register;
  LPC_SC_TypeDef local_sc;

  memset(&local_uart0_register, 0, sizeof(local_uart0_register));
  memset(&local_sc, 0, sizeof(local_sc));

  Mock<PinInterface> mock_gpio_rx;
  Fake(Method(mock_gpio_rx, SetPinFunction));
  Fake(Method(mock_gpio_rx, SetMode));
  Mock<PinInterface> mock_gpio_tx;
  Fake(Method(mock_gpio_tx, SetPinFunction));
  Fake(Method(mock_gpio_tx, SetMode));

  uart0::uart0_register = &local_uart0_register;
  uart0::sc             = &local_sc;

  uart0::rx = &mock_gpio_rx.get();
  uart0::tx = &mock_gpio_tx.get();

  SECTION("Initialize")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 84 page 133
    constexpr uint32_t kPconpUart0  = 1 << 3;
    constexpr uint32_t kFIfoEnabled = 1 << 0;
    constexpr uint32_t kBaudRate    = 500'000;
    // 12'000'000 Hz / (16*1*(1+1/2)) = 500'000;
    // constexpr uint32_t kExpectedUpperByte = 0;
    // constexpr uint32_t kExpectedLowerByte = 1;
    // constexpr uint32_t kExpectedDivAdd    = 1;
    // constexpr uint32_t kExpectedMul       = 2;
    // constexpr uint32_t kExpectedFdr =
    //                       (kExpectedMul << 4) | kExpectedDivAdd;
    constexpr uint32_t kBits8DlabCleared = 3;

    uart0::Init(kBaudRate);
    CHECK(kPconpUart0 == (kPconpUart0 & local_sc.PCONP));

    Verify(Method(mock_gpio_tx, SetMode).Using(PinInterface::Mode::kPullUp))
        .Once();
    Verify(Method(mock_gpio_tx, SetPinFunction).Using(1)).Once();
    Verify(Method(mock_gpio_rx, SetMode).Using(PinInterface::Mode::kPullUp))
        .Once();
    Verify(Method(mock_gpio_rx, SetPinFunction).Using(1)).Once();

    // CHECK(kExpectedUpperByte == local_uart0_register.DLM);
    // CHECK(kExpectedLowerByte == local_uart0_register.DLL);
    CHECK(kBits8DlabCleared == local_uart0_register.LCR);
    CHECK(kFIfoEnabled == (local_uart0_register.FCR & kFIfoEnabled));
    // CHECK(kExpectedFdr == local_uart0_register.FDR);
  }
}

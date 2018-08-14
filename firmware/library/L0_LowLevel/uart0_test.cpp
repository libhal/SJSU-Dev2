#include <cstdint>

#include "config.hpp"
#include "LPC40xx.h"
#include "L0_LowLevel/uart0.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing Uart0", "[uart0]")
{
    using fakeit::Mock;
    using fakeit::Fake;
    using fakeit::Verify;

    LPC_UART_TypeDef local_uart0_register;
    LPC_SC_TypeDef local_sc;

    memset(&local_uart0_register, 0, sizeof(local_uart0_register));
    memset(&local_sc, 0, sizeof(local_sc));

    Mock<PinConfigureInterface> mock_gpio_rx;
    Fake(Method(mock_gpio_rx, SetPinFunction));
    Fake(Method(mock_gpio_rx, SetPinMode));
    Mock<PinConfigureInterface> mock_gpio_tx;
    Fake(Method(mock_gpio_tx, SetPinFunction));
    Fake(Method(mock_gpio_tx, SetPinMode));

    uart0::uart0_register = &local_uart0_register;
    uart0::sc             = &local_sc;

    uart0::rx = &mock_gpio_rx.get();
    uart0::tx = &mock_gpio_tx.get();

    SECTION("Initialize")
    {
        // Source: "UM10562 LPC408x/407x User manual" table 84 page 133
        constexpr uint32_t kPconpUart0  = 1 << 3;
        constexpr uint32_t kFIfoEnabled = 1 << 0;
        constexpr uint32_t kBaudRate    = 38400;
        constexpr uint32_t kDivider     = static_cast<uint32_t>(
            config::kSystemClockRate / (16.0f * kBaudRate) + 0.5f);
        constexpr uint32_t kUpperByte        = (kDivider >> 8) & 0xFF;
        constexpr uint32_t kLowerByte        = kDivider & 0xFF;
        constexpr uint32_t kBits8DlabCleared = 3;

        uart0::Init(kBaudRate);
        CHECK(kPconpUart0 == (kPconpUart0 & local_sc.PCONP));

        Verify(Method(mock_gpio_tx, SetPinMode)
                   .Using(PinConfigureInterface::PinMode::kPullUp))
            .Once();
        Verify(Method(mock_gpio_tx, SetPinFunction).Using(1)).Once();
        Verify(Method(mock_gpio_rx, SetPinMode)
                   .Using(PinConfigureInterface::PinMode::kPullUp))
            .Once();
        Verify(Method(mock_gpio_rx, SetPinFunction).Using(1)).Once();

        CHECK(kUpperByte == local_uart0_register.DLM);
        CHECK(kLowerByte == local_uart0_register.DLL);
        CHECK(kBits8DlabCleared == local_uart0_register.LCR);
        CHECK(kFIfoEnabled == (local_uart0_register.FCR & kFIfoEnabled));
    }
}

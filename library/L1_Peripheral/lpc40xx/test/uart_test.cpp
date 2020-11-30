#include "L1_Peripheral/lpc40xx/uart.hpp"

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Uart);

TEST_CASE("Testing lpc40xx Uart")
{
  // Simulated local version of LPC_UART2 to verify registers
  LPC_UART_TypeDef local_uart;
  testing::ClearStructure(&local_uart);

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kDummySystemControllerClockFrequency =
      48_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetClockRate))
      .AlwaysReturn(kDummySystemControllerClockFrequency);

  sjsu::SystemController::SetPlatformController(&mock_system_controller.get());

  Mock<sjsu::Pin> mock_tx;
  Fake(Method(mock_tx, ConfigureFunction));
  Fake(Method(mock_tx, ConfigurePullResistor));
  Mock<sjsu::Pin> mock_rx;
  Fake(Method(mock_rx, ConfigureFunction));
  Fake(Method(mock_rx, ConfigurePullResistor));

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

  Uart uart_test(kMockUart2);

  SECTION("Initialize()")
  {
    // Exercise
    uart_test.ModuleInitialize();

    // Verify
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching([](sjsu::SystemController::ResourceID id) {
                 return sjsu::lpc40xx::SystemController::Peripherals::kUart2
                            .device_id == id.device_id;
               }));
  }

  SECTION("Enable()")
  {
    // Exercise
    uart_test.ModuleEnable();

    // Verify
    Verify(Method(mock_tx, ConfigureFunction).Using(kMockUart2.tx_function_id))
        .Once();
    Verify(Method(mock_rx, ConfigureFunction).Using(kMockUart2.rx_function_id))
        .Once();
    Verify(Method(mock_tx, ConfigurePullResistor)
               .Using(sjsu::Pin::Resistor::kPullUp))
        .Once();
    Verify(Method(mock_rx, ConfigurePullResistor)
               .Using(sjsu::Pin::Resistor::kPullUp))
        .Once();

    CHECK(0b111 == bit::Extract(local_uart.FCR, bit::MaskFromRange(0, 2)));
  }

  SECTION("Enable(false)")
  {
    // Exercise
    uart_test.ModuleEnable(false);

    // Verify
    CHECK(0b000 == bit::Extract(local_uart.FCR, bit::MaskFromRange(0, 2)));
  }

  SECTION("ConfigureBaudRate(9600)")
  {
    // Setup
    constexpr uint32_t kBaudRate          = 9600;
    constexpr uint32_t kBits8DlabClear    = 3;
    constexpr uint32_t kExpectedUpperByte = 0;
    constexpr uint32_t kExpectedLowerByte = 208;
    constexpr uint32_t kExpectedDivAdd    = 1;
    constexpr uint32_t kExpectedMul       = 2;
    constexpr uint32_t kExpectedFdr = (kExpectedMul << 4) | kExpectedDivAdd;

    // Exercise
    uart_test.ConfigureBaudRate(kBaudRate);

    // Verify
    CHECK(kExpectedUpperByte == local_uart.DLM);
    CHECK(kExpectedLowerByte == local_uart.DLL);
    CHECK(kExpectedFdr == local_uart.FDR);
    CHECK(kBits8DlabClear == local_uart.LCR);
  }

  sjsu::lpc40xx::SystemController::system_controller = LPC_SC;
}
}  // namespace sjsu::lpc40xx

#include "peripherals/lpc40xx/uart.hpp"

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
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
  Mock<sjsu::Pin> mock_rx;
  Fake(Method(mock_tx, Pin::ModuleInitialize));
  Fake(Method(mock_rx, Pin::ModuleInitialize));

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

  Uart test_subject(kMockUart2);

  SECTION("Initialize()")
  {
    // Setup
    mock_tx.get().settings = PinSettings_t{};
    mock_rx.get().settings = PinSettings_t{};

    // Setup: Baud Rate
    SECTION("baudrate = 9600")
    {
      test_subject.settings.baud_rate = 9600;
    }
    SECTION("baudrate = 38400")
    {
      test_subject.settings.baud_rate = 38400;
    }
    SECTION("baudrate = 115200")
    {
      test_subject.settings.baud_rate = 115200;
    }
    SECTION("baudrate = 512000")
    {
      test_subject.settings.baud_rate = 512000;
    }
    SECTION("baudrate = 1'000'000")
    {
      test_subject.settings.baud_rate = 1'000'000;
    }

    constexpr uint32_t kBits8DlabClear = 3;

    const uart::UartCalibration_t kCalibration = uart::GenerateUartCalibration(
        test_subject.settings.baud_rate, kDummySystemControllerClockFrequency);

    const uint8_t kExpectedUpperByte =
        static_cast<uint8_t>((kCalibration.divide_latch >> 8) & 0xFF);
    const uint8_t kExpectedLowerByte =
        static_cast<uint8_t>(kCalibration.divide_latch & 0xFF);
    const uint8_t kExpectedFdr = static_cast<uint8_t>(
        (kCalibration.multiply & 0xF) << 4 | (kCalibration.divide_add & 0xF));

    // Exercise
    test_subject.Initialize();

    // Verify
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching([](sjsu::SystemController::ResourceID id) {
                 return sjsu::lpc40xx::SystemController::Peripherals::kUart2
                            .device_id == id.device_id;
               }));

    // Verify
    CHECK(mock_tx.get().CurrentSettings() ==
          PinSettings_t{
              .function = kMockUart2.tx_function_id,
              .resistor = sjsu::PinSettings_t::Resistor::kPullUp,
          });

    CHECK(mock_rx.get().CurrentSettings() ==
          PinSettings_t{
              .function = kMockUart2.rx_function_id,
              .resistor = sjsu::PinSettings_t::Resistor::kPullUp,
          });

    CHECK(0b111 == bit::Extract(local_uart.FCR, bit::MaskFromRange(0, 2)));

    // Verify
    CHECK(kExpectedUpperByte == local_uart.DLM);
    CHECK(kExpectedLowerByte == local_uart.DLL);
    CHECK(kExpectedFdr == local_uart.FDR);
    CHECK(kBits8DlabClear == local_uart.LCR);
  }

  SECTION("PowerDown()")
  {
    // Setup
    local_uart.FCR =
        bit::Insert(0b111, local_uart.FCR, bit::MaskFromRange(0, 2));

    // Exercise
    test_subject.PowerDown();

    // Verify
    CHECK(0b000 == bit::Extract(local_uart.FCR, bit::MaskFromRange(0, 2)));
  }

  sjsu::lpc40xx::SystemController::system_controller = LPC_SC;
}
}  // namespace sjsu::lpc40xx

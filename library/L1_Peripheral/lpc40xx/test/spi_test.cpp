// this is the ssp.hpp test file

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/spi.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Spi);

TEST_CASE("Testing lpc40xx SPI", "[lpc40xx-Spi]")
{
  // Simulate local version of LPC_SSP
  LPC_SSP_TypeDef local_ssp;
  // Clear memory locations
  memset(&local_ssp, 0, sizeof(local_ssp));

  // Set mock for sjsu::SystemController
  constexpr units::frequency::hertz_t kDummySystemControllerClockFrequency =
      12_MHz;
  Mock<sjsu::SystemController> mock_system_controller;
  Fake(Method(mock_system_controller, PowerUpPeripheral));
  When(Method(mock_system_controller, GetSystemFrequency))
      .AlwaysReturn(kDummySystemControllerClockFrequency);
  When(Method(mock_system_controller, GetPeripheralClockDivider))
      .AlwaysReturn(1);

  // Set up Mock for PinCongiure
  Mock<sjsu::Pin> mock_mosi;
  Mock<sjsu::Pin> mock_miso;
  Mock<sjsu::Pin> mock_sck;

  Fake(Method(mock_mosi, SetPinFunction));
  Fake(Method(mock_miso, SetPinFunction));
  Fake(Method(mock_sck, SetPinFunction));

  // Set up SSP Bus configuration object
  const Spi::Bus_t kMockSpi = {
    .registers       = &local_ssp,
    .power_on_bit    = sjsu::lpc40xx::SystemController::Peripherals::kSsp0,
    .mosi            = mock_mosi.get(),
    .miso            = mock_miso.get(),
    .sck             = mock_sck.get(),
    .pin_function_id = 0b110,
  };

  Spi test_spi(kMockSpi, mock_system_controller.get());
  test_spi.SetSpiDefault();
  test_spi.Initialize();

  SECTION("Initialize")
  {
    Verify(Method(mock_system_controller, PowerUpPeripheral)
               .Matching([](sjsu::SystemController::PeripheralID id) {
                 return sjsu::lpc40xx::SystemController::Peripherals::kSsp0
                            .device_id == id.device_id;
               }));

    Verify(Method(mock_mosi, SetPinFunction).Using(kMockSpi.pin_function_id))
        .Once();
    Verify(Method(mock_miso, SetPinFunction).Using(kMockSpi.pin_function_id))
        .Once();
    Verify(Method(mock_sck, SetPinFunction).Using(kMockSpi.pin_function_id))
        .Once();

    constexpr uint8_t kSspEnable = 0b10;
    CHECK((local_ssp.CR1 & kSspEnable) == kSspEnable);
  }

  SECTION("Verify Mode and Frame")
  {
    constexpr uint8_t kSlaveBit = 2;
    constexpr uint8_t kDataBit  = 0;

    // Check that slave mode bit is 0, meaning we are in master mode.
    CHECK(bit::Read(local_ssp.CR1, kSlaveBit) == false);
    CHECK((local_ssp.CR0 & (0xF << kDataBit)) ==
          Value(Spi::DataSize::kEight) + 3);
  }

  SECTION("Verify Clock Polarity and Prescaler")
  {
    constexpr uint8_t kPolarityMask = 0 << 6;
    constexpr uint16_t kPrescaler =
        (kDummySystemControllerClockFrequency / 1_MHz).to<uint16_t>();

    CHECK((local_ssp.CR0 & kPolarityMask) == kPolarityMask);
    CHECK(local_ssp.CPSR == (kPrescaler & 0xFF));
  }

  SECTION("Check Transfer Register")
  {
    constexpr uint8_t kIdle    = 0;
    constexpr uint8_t kIdleBit = 4;

    CHECK((local_ssp.SR & (0x1 << kIdleBit)) == kIdle);
  }

  sjsu::lpc40xx::SystemController::system_controller = LPC_SC;
}
}  // namespace sjsu::lpc40xx

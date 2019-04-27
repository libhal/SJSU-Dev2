// this is the ssp.hpp test file

#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/spi.hpp"
#include "L4_Testing/testing_frameworks.hpp"

EMIT_ALL_METHODS(Spi);

TEST_CASE("Testing SPI", "[Spi]")
{
  // Simulate local version of LPC_SSP
  LPC_SSP_TypeDef local_ssp;
  LPC_SC_TypeDef local_sc;

  Lpc40xxSystemController::system_controller = &local_sc;

  // Clear memory locations
  memset(&local_ssp, 0, sizeof(local_ssp));
  memset(&local_sc, 0, sizeof(local_sc));

  // Set up Mock for PinCongiure
  Mock<PinInterface> mock_mosi;
  Mock<PinInterface> mock_miso;
  Mock<PinInterface> mock_sck;

  Fake(Method(mock_mosi, SetPinFunction));
  Fake(Method(mock_miso, SetPinFunction));
  Fake(Method(mock_sck, SetPinFunction));

  // Set up SSP Bus configuration object
  const Spi::Bus_t kMockSpi = {
    .registers       = &local_ssp,
    .power_on_bit    = Lpc40xxSystemController::Peripherals::kSsp0,
    .mosi            = mock_mosi.get(),
    .miso            = mock_miso.get(),
    .sck             = mock_sck.get(),
    .pin_function_id = 0b110,
  };

  Spi test_spi(kMockSpi);
  test_spi.SetSpiMasterDefault();
  test_spi.Initialize();

  SECTION("Initialize")
  {
    CHECK(Lpc40xxSystemController().IsPeripheralPoweredUp(
        Lpc40xxSystemController::Peripherals::kSsp0));

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
    constexpr uint8_t kMasterBit = 2;
    constexpr uint8_t kDataBit   = 0;

    CHECK((local_ssp.CR1 & (0x1 << kMasterBit)) ==
          util::Value(SpiInterface::MasterSlaveMode::kMaster));
    CHECK((local_ssp.CR0 & (0xF << kDataBit)) ==
          util::Value(SpiInterface::DataSize::kEight));
  }

  SECTION("Verify Clock Polarity and Prescaler")
  {
    constexpr uint8_t kPolarityBit = 6;
    constexpr uint8_t kPolarity    = 1;
    constexpr uint8_t kPrescaler   = config::kSystemClockRateMhz;

    CHECK(((local_ssp.CR0 & (0x1 << kPolarityBit)) >> kPolarityBit) ==
          kPolarity);
    CHECK(local_ssp.CPSR == kPrescaler);
  }

  SECTION("Check Transfer Register")
  {
    constexpr uint8_t kIdle    = 0;
    constexpr uint8_t kIdleBit = 4;

    CHECK((local_ssp.SR & (0x1 << kIdleBit)) == kIdle);
  }

  Lpc40xxSystemController::system_controller = LPC_SC;
}

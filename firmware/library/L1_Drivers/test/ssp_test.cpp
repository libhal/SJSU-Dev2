// this is the ssp.hpp test file

#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/ssp.hpp"
#include "L4_Testing/testing_frameworks.hpp"

EMIT_ALL_METHODS(Ssp);

TEST_CASE("Testing SSP", "[Ssp]")
{
  // Simulate local version of LPC_SSP
  LPC_SSP_TypeDef local_ssp[3];
  LPC_SC_TypeDef local_sc;
  LPC_IOCON_TypeDef local_iocon;

  // Clear memory locations
  memset(&local_ssp, 0, sizeof(local_ssp));
  memset(&local_sc, 0, sizeof(local_sc));
  memset(&local_iocon, 0, sizeof(local_iocon));

  // Set up SSP
  Ssp::ssp_registers[0] = &local_ssp[0];
  Lpc40xxSystemController::system_controller = &local_sc;
  Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(&local_iocon);

  // Set up Mock for PinCongiure
  Mock<PinInterface> mock_mosi;
  Mock<PinInterface> mock_miso;
  Mock<PinInterface> mock_sck;

  Fake(Method(mock_mosi, SetPinFunction));
  Fake(Method(mock_miso, SetPinFunction));
  Fake(Method(mock_sck, SetPinFunction));

  PinInterface & mosi = mock_mosi.get();
  PinInterface & miso = mock_miso.get();
  PinInterface & sck = mock_sck.get();

  Ssp test_ssp(Ssp::Peripheral::kSsp0, &mosi, &miso, &sck);
  test_ssp.SetSpiMasterDefault();
  test_ssp.Initialize();

  SECTION("Verify Mode and Frame")
  {
    constexpr uint8_t kMasterBit = 2;
    constexpr uint8_t kDataBit = 0;
    constexpr uint8_t kFrameBit = 4;

    CHECK((local_ssp[0].CR1 & (0x1 << kMasterBit)) ==
      SspInterface::MasterSlaveMode::kMaster);
    CHECK((local_ssp[0].CR0 & (0x3 << kFrameBit)) ==
      SspInterface::FrameMode::kSpi);
    CHECK((local_ssp[0].CR0 & (0xF << kDataBit)) ==
      SspInterface::DataSize::kEight);
  }

  SECTION("Verify Clock Polarity and Prescaler")
  {
    constexpr uint8_t kPolarityBit = 6;
    constexpr uint8_t kPolarity = 1;
    constexpr uint8_t kPrescaler = config::kSystemClockRateMhz;

    CHECK(((local_ssp[0].CR0 & (0x1 << kPolarityBit)) >> kPolarityBit)
      == kPolarity);
    CHECK(local_ssp[0].CPSR == kPrescaler);
  }

  SECTION("Check Transfer Register")
  {
    constexpr uint8_t kIdle = 0;
    constexpr uint8_t kIdleBit = 4;

    CHECK((local_ssp[0].SR & (0x1 << kIdleBit)) == kIdle);
  }

  Ssp::ssp_registers[0] = LPC_SSP0;
  Ssp::ssp_registers[1] = LPC_SSP1;
  Ssp::ssp_registers[2] = LPC_SSP2;
  Lpc40xxSystemController::system_controller = LPC_SC;
  Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(LPC_IOCON);
}

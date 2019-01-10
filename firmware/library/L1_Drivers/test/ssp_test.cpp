// this is the ssp.hpp test file

#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/ssp.hpp"
#include "L4_Testing/testing_frameworks.hpp"

EMIT_ALL_METHODS(Ssp);

TEST_CASE("Testing SSP", "[Ssp]")
{
  // Simulate local version of LPC_SSP
  LPC_SSP_TypeDef local_ssp;
  LPC_SC_TypeDef local_sc;

  // Clear memory locations
  memset(&local_ssp, 0, sizeof(local_ssp));
  memset(&local_sc, 0, sizeof(local_sc));

  // Set up Mock for PinCongiure
  Mock<PinInterface> mock_mosi;
  Mock<PinInterface> mock_miso;
  Mock<PinInterface> mock_sck;

  Fake(Method(mock_mosi, CreateInactivePin));
  Fake(Method(mock_miso, CreateInactivePin));
  Fake(Method(mock_sck, CreateInactivePin));

  // Set up SSP
  Ssp::ssp_registers[0] = &local_ssp;
  Lpc40xxSystemController::system_controller = &local_sc;

  PinInterface &mosi = mock_mosi.get();
  PinInterface &miso = mock_miso.get();
  PinInterface &sck = mock_sck.get();

  Ssp test_ssp(Ssp::Peripherals::kSsp0, mosi, miso, sck);
  test_ssp.SetSpiMasterDefault();
  test_ssp.Initialize();

  SECTION("Verification")
  {
  }
}

#include "L1_Drivers/dac.hpp"
#include "L1_Drivers/pin.hpp"
#include "L5_Testing/testing_frameworks.hpp"
TEST_CASE("Testing Dac", "[dac]")
{
  using fakeit::Fake;
  using fakeit::Mock;
  using fakeit::Spy;
  using fakeit::Using;
  using fakeit::Verify;
  using fakeit::When;

  LPC_IOCON_TypeDef local_iocon;
  memset(&local_iocon, 0, sizeof(local_iocon));
  LPC_DAC_TypeDef local_dac_port;
  Dac::dac_register = &local_dac_port;
  Pin iocon_pin_config(0, 26);
  Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(&local_iocon);
  Pin test_pin(0, 26);
  Mock<PinInterface> mock_dac_pin;
  Fake(Method(mock_dac_pin, SetPinFunction), Method(mock_dac_pin, EnableDac),
       Method(mock_dac_pin, SetAsAnalogMode), Method(mock_dac_pin, SetMode));
  PinInterface & dac = mock_dac_pin.get();
  Dac test_subject00(&dac);
  Dac test_subject01;
  SECTION("Initialize Dac")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 686 page 814
    constexpr uint8_t kBiasMask = 0b0;
    local_dac_port.CR           = kBiasMask;
    // Mocked out Initialize for the Verify Methods
    test_subject00.Initialize();
    // Initialize that can properly check the registers if they
    // recieved the correct values.
    test_subject01.Initialize();
    // Check Pin Mode DAC_OUT
    Verify(Method(mock_dac_pin, SetPinFunction).Using(Dac::kDacMode),
           Method(mock_dac_pin, EnableDac).Using(true),
           Method(mock_dac_pin, SetAsAnalogMode).Using(true),
           Method(mock_dac_pin, SetMode).Using(PinInterface::Mode::kInactive));
  }
  SECTION("Write Dac")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 686 page 814
    constexpr uint16_t kBiasMask = 0b0000000000;
    local_dac_port.CR            = kBiasMask;
    // Test the WriteDac function by checking if the CR
    // register receives the correctValue when the
    // function has its value input.
    constexpr uint8_t kDacWrite0 = 10;
    constexpr uint8_t kDacWrite1 = 100;
    constexpr uint32_t kMask     = kDacWrite0 << Dac::kDacOutReg;
    constexpr uint32_t kMask1    = kDacWrite1 << Dac::kDacOutReg;
    test_subject00.WriteDac(10);
    CHECK((kMask) == (local_dac_port.CR & kMask));
    test_subject00.WriteDac(100);
    CHECK((kMask1) == (local_dac_port.CR & kMask1));
  }
  SECTION("SetVoltage")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 686 page 814
    constexpr float kVoltageInput0 = 3;
    constexpr float kDacVoltage0   = (kVoltageInput0 * 1024.0f) / 3.3f;
    constexpr int kConversion0     = static_cast<int>(kDacVoltage0);
    constexpr uint32_t kMask       = kConversion0 << Dac::kDacOutReg;
    test_subject00.SetVoltage(kVoltageInput0);
    CHECK((kMask) == (local_dac_port.CR & kMask));
  }
  SECTION("SetBias")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 686 page 814
    constexpr uint8_t kBiasMask = 0b0;
    constexpr uint32_t kMask    = 0b1 << Dac::kBiasReg;

    local_dac_port.CR = kBiasMask;
    // Test Setting Bias to 0 and checking if the register is correct
    test_subject00.SetBias(Dac::Bias::kHigh);
    CHECK(0 == (local_dac_port.CR & kMask));
    // Test Setting Bias to 1 and checking if the register is correct
    test_subject00.SetBias(Dac::Bias::kLow);
    CHECK(kMask == (local_dac_port.CR & kMask));
  }
  Dac::dac_register = LPC_DAC;
  Pin::pin_map      = reinterpret_cast<Pin::PinMap_t *>(LPC_IOCON);
}

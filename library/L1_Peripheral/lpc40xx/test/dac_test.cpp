#include "L1_Peripheral/lpc40xx/dac.hpp"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu::lpc40xx
{
EMIT_ALL_METHODS(Dac);

TEST_CASE("Testing lpc40xx Dac", "[lpc40xx-dac]")
{
  LPC_IOCON_TypeDef local_iocon;
  LPC_DAC_TypeDef local_dac_port;

  memset(&local_iocon, 0, sizeof(local_iocon));
  memset(&local_dac_port, 0, sizeof(local_dac_port));

  Dac::dac_register = &local_dac_port;

  Mock<sjsu::Pin> mock_dac_pin;
  Fake(Method(mock_dac_pin, SetPinFunction),
       Method(mock_dac_pin, SetAsAnalogMode), Method(mock_dac_pin, SetPull));
  // Substitute the memory mapped LPC_IOCON with the local_iocon test struture
  // Redirects manipulation to the 'local_iocon'
  // This is necessary because we have to cast the pin interface back to a Pin
  // object which will attempt to manipulate hardware registers.
  Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(&local_iocon);

  Dac test_subject(mock_dac_pin.get());

  SECTION("Initialize Dac")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 686 page 814
    constexpr uint8_t kBiasMask = 0b0;
    constexpr uint8_t kDacMode  = 0b010;
    local_dac_port.CR           = kBiasMask;
    // Mocked out Initialize for the Verify Methods
    test_subject.Initialize();
    // Check Pin Mode DAC_OUT
    Verify(Method(mock_dac_pin, SetPinFunction).Using(kDacMode),
           Method(mock_dac_pin, SetAsAnalogMode).Using(true),
           Method(mock_dac_pin, SetPull).Using(sjsu::Pin::Resistor::kNone));
    // Check that DacEnable occured!
    // local_iocon.
  }
  SECTION("Write Dac")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 686 page 814
    constexpr uint16_t kBiasMask = 0b0000000000;
    local_dac_port.CR            = kBiasMask;
    // Test the Write function by checking if the CR
    // register receives the correctValue when the
    // function has its value input.
    constexpr uint8_t kDacWrite0 = 10;
    constexpr uint8_t kDacWrite1 = 100;
    constexpr uint32_t kMask0    = kDacWrite0 << Dac::kDacOutReg;
    constexpr uint32_t kMask1    = kDacWrite1 << Dac::kDacOutReg;
    test_subject.Write(10);
    CHECK(kMask0 == (local_dac_port.CR & kMask0));
    test_subject.Write(100);
    CHECK(kMask1 == (local_dac_port.CR & kMask1));
  }
  SECTION("SetVoltage")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 686 page 814
    constexpr float kVoltageInput0 = 3;
    constexpr float kDacVoltage0   = (kVoltageInput0 * 1024.0f) / 3.3f;
    constexpr int kConversion0     = static_cast<int>(kDacVoltage0);
    constexpr uint32_t kMask       = kConversion0 << Dac::kDacOutReg;
    test_subject.SetVoltage(kVoltageInput0);
    CHECK(kMask == (local_dac_port.CR & kMask));
  }
  SECTION("SetBias")
  {
    constexpr uint32_t kMask = 0b1 << Dac::kBiasReg;
    // Test Setting Bias to 0 and checking if the register is correct
    test_subject.SetBias(Dac::Bias::kHigh);
    CHECK(0 == (local_dac_port.CR & kMask));
    // Test Setting Bias to 1 and checking if the register is correct
    test_subject.SetBias(Dac::Bias::kLow);
    CHECK(kMask == (local_dac_port.CR & kMask));
  }

  Dac::dac_register = LPC_DAC;
  Pin::pin_map      = reinterpret_cast<Pin::PinMap_t *>(LPC_IOCON);
}
}  // namespace sjsu::lpc40xx

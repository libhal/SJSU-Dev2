#include "L1_Peripheral/lpc40xx/dac.hpp"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/bit.hpp"

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
       Method(mock_dac_pin, SetAsAnalogMode),
       Method(mock_dac_pin, SetPull));
  // Substitute the memory mapped LPC_IOCON with the local_iocon test struture
  // Redirects manipulation to the 'local_iocon'
  // This is necessary because we have to cast the pin interface back to a Pin
  // object which will attempt to manipulate hardware registers.
  Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(&local_iocon);

  Dac test_subject(mock_dac_pin.get());

  SECTION("Initialize Dac")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 686 page 814
    constexpr uint8_t kDacMode  = 0b010;
    // Mocked out Initialize for the Verify Methods
    test_subject.Initialize();
    // Check Pin Mode DAC_OUT
    Verify(Method(mock_dac_pin, SetPinFunction).Using(kDacMode),
           Method(mock_dac_pin, SetAsAnalogMode).Using(true),
           Method(mock_dac_pin, SetPull).Using(sjsu::Pin::Resistor::kNone));

    CHECK(0 == bit::Read(local_dac_port.CR, Dac::Control::kBias));
  }
  SECTION("Write Dac")
  {
    // Test the Write function by checking if the CR
    // register receives the correctValue when the
    // function has its value input.
    constexpr uint8_t kDacWrite0 = 10;
    constexpr uint8_t kDacWrite1 = 100;
    test_subject.Write(10);
    CHECK(kDacWrite0 == bit::Extract(local_dac_port.CR, Dac::Control::kValue));
    test_subject.Write(100);
    CHECK(kDacWrite1 == bit::Extract(local_dac_port.CR, Dac::Control::kValue));
  }
  SECTION("SetVoltage")
  {
    // Setup
    // Source: "UM10562 LPC408x/407x User manual" table 686 page 814
    constexpr float kVoltageInput0 = 3.0f;
    constexpr float kDacVoltage0   = (kVoltageInput0 * 1024.0f) / 3.3f;
    constexpr int kConversion0     = static_cast<int>(kDacVoltage0);
    // Exercise
    test_subject.SetVoltage(kVoltageInput0);
    // Verify
    CHECK(kConversion0 ==
          bit::Extract(local_dac_port.CR, Dac::Control::kValue));
  }
  SECTION("SetBias(High)")
  {
    // Exercise
    test_subject.SetBias(Dac::Bias::kHigh);
    // Verify
    CHECK(!bit::Read(local_dac_port.CR, Dac::Control::kBias));
  }
  SECTION("SetBias(Low)")
  {
    // Exercise
    test_subject.SetBias(Dac::Bias::kLow);
    // Verify
    CHECK(bit::Read(local_dac_port.CR, Dac::Control::kBias));
  }

  Dac::dac_register = LPC_DAC;
  Pin::pin_map      = reinterpret_cast<Pin::PinMap_t *>(LPC_IOCON);
}
}  // namespace sjsu::lpc40xx

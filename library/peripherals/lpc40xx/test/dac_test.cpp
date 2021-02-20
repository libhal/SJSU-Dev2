#include "peripherals/lpc40xx/dac.hpp"

#include "peripherals/lpc40xx/pin.hpp"
#include "testing/testing_frameworks.hpp"
#include "utility/math/bit.hpp"

namespace sjsu::lpc40xx
{
TEST_CASE("Testing lpc40xx Dac")
{
  LPC_IOCON_TypeDef local_iocon;
  testing::ClearStructure(&local_iocon);
  // Substitute the memory mapped LPC_IOCON with the local_iocon test struture
  // Redirects manipulation to the 'local_iocon'
  // This is necessary because we have to cast the pin interface back to a Pin
  // object which will attempt to manipulate hardware registers.
  Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(&local_iocon);

  LPC_DAC_TypeDef local_dac_port;
  testing::ClearStructure(&local_dac_port);
  Dac::dac_register = &local_dac_port;

  Mock<sjsu::Pin> mock_dac_pin;
  Fake(Method(mock_dac_pin, Pin::ModuleInitialize));

  Dac test_subject(mock_dac_pin.get());

  SECTION("Initialize Dac")
  {
    // Source: "UM10562 LPC408x/407x User manual" table 686 page 814
    constexpr uint8_t kDacMode                         = 0b010;
    constexpr sjsu::PinSettings_t kExpectedPinSettings = {
      .function   = kDacMode,
      .resistor   = sjsu::PinSettings_t::Resistor::kNone,
      .open_drain = false,
      .as_analog  = true,
    };

    // Mocked out Initialize for the Verify Methods
    test_subject.Initialize();

    // Check Pin Mode DAC_OUT
    CHECK(mock_dac_pin.get().CurrentSettings() == kExpectedPinSettings);

    Verify(Method(mock_dac_pin, Pin::ModuleInitialize)).Once();

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
    constexpr auto kVoltageInput0     = 3_V;
    constexpr auto kVoltageInputFloat = kVoltageInput0.to<float>();
    constexpr float kDacVoltage0      = (kVoltageInputFloat * 1024.0f) / 3.3f;
    constexpr int kConversion0        = static_cast<int>(kDacVoltage0);

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

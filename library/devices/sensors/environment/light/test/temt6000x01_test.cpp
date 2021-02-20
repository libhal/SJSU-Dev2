#include "devices/sensors/environment/light/temt6000x01.hpp"

#include "testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing TEMP6000X01 Light Sensor")
{
  constexpr units::voltage::microvolt_t kReferenceVoltage = 3.3_V;
  constexpr units::impedance::ohm_t kPullDownResistor     = 10'000_Ohm;
  constexpr uint8_t kAdcActiveBits                        = 12;
  constexpr uint32_t kAdcResolution                       = 4'095;
  constexpr uint32_t kAdcTestOutput                       = 1'024;

  constexpr units::voltage::microvolt_t kExpectedVoltage(
      kAdcTestOutput * (kReferenceVoltage / kAdcResolution));
  constexpr units::current::microampere_t kExpectedCurrent(kExpectedVoltage /
                                                           kPullDownResistor);

  units::illuminance::lux_t expected_lux(2.0f * kExpectedCurrent.to<float>());
  units::illuminance::lux_t expected_max_lux(1'000_lx);

  Mock<Adc> mock_adc;
  mock_adc.get().settings.reference_voltage = 3.3_V;
  When(Method(mock_adc, Adc::GetActiveBits)).AlwaysReturn(kAdcActiveBits);
  When(Method(mock_adc, Adc::Read)).AlwaysReturn(kAdcTestOutput);

  Temt6000x01 test_subject(mock_adc.get(), kPullDownResistor);

  // Setup
  Fake(Method(mock_adc, ModuleInitialize));

  // Exercise
  test_subject.Initialize();

  SECTION("GetIlluminance")
  {
    // Exercise
    auto actual_lux = test_subject.GetIlluminance();

    // Verify
    Verify(Method(mock_adc, Read)).Once();

    CHECK(actual_lux.to<float>() ==
          doctest::Approx(expected_lux.to<float>()).epsilon(0.01f));
  }

  SECTION("GetMaxIlluminance")
  {
    // Exercise
    auto actual_max_lux = test_subject.GetMaxIlluminance();

    // Verify
    CHECK(actual_max_lux.to<float>() ==
          doctest::Approx(expected_max_lux.to<float>()).epsilon(0.01f));
  }

  SECTION("GetIlluminancePercentage")
  {
    float expected_percentage = (expected_lux / expected_max_lux).to<float>();

    // Exercise
    float actual_percentage = test_subject.GetPercentageBrightness();

    // Verify
    Verify(Method(mock_adc, Read)).Once();
    CHECK(actual_percentage ==
          doctest::Approx(expected_percentage).epsilon(0.01f));
  }

  // Verify
  Verify(Method(mock_adc, ModuleInitialize)).Once();
}
}  // namespace sjsu

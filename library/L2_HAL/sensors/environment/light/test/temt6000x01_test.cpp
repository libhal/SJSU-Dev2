#include "L2_HAL/sensors/environment/light/temt6000x01.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Temt6000x01);

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
  When(Method(mock_adc, GetActiveBits)).AlwaysReturn(kAdcActiveBits);
  When(Method(mock_adc, Read)).AlwaysReturn(kAdcTestOutput);
  When(Method(mock_adc, ReferenceVoltage)).AlwaysReturn(kReferenceVoltage);

  Temt6000x01 test_subject(mock_adc.get(), kPullDownResistor);

  SECTION("Successful Initialize")
  {
    // Setup
    When(Method(mock_adc, Initialize)).AlwaysReturn({});

    // Exercise
    REQUIRE(test_subject.Initialize());

    // Verify
    Verify(Method(mock_adc, Initialize)).Once();
  }

  SECTION("Successful Initialize")
  {
    const auto kExpectedError = std::errc::invalid_argument;
    When(Method(mock_adc, Initialize)).AlwaysReturn(Error(kExpectedError, ""));

    // Exercise
    REQUIRE(kExpectedError == test_subject.Initialize());

    // Verify
    Verify(Method(mock_adc, Initialize)).Once();
  }

  SECTION("GetIlluminance")
  {
    // Exercise
    auto actual_lux = test_subject.GetIlluminance().value();

    // Verify
    Verify(Method(mock_adc, Read)).Once();
    CHECK(actual_lux == expected_lux);
  }

  SECTION("GetMaxIlluminance")
  {
    // Exercise
    auto actual_max_lux = test_subject.GetMaxIlluminance().value();

    // Verify
    CHECK(actual_max_lux == expected_max_lux);
  }

  SECTION("GetIlluminancePercentage")
  {
    float expected_percentage = (expected_lux / expected_max_lux).to<float>();

    // Exercise
    float actual_percentage = test_subject.GetPercentageBrightness().value();

    // Verify
    Verify(Method(mock_adc, Read)).Once();
    CHECK(actual_percentage ==
          doctest::Approx(expected_percentage).epsilon(0.01f));
  }
}
}  // namespace sjsu

#include "L2_HAL/sensors/environment/light/temt6000x01.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Temt6000x01);

TEST_CASE("Testing TEMP6000X01 Light Sensor", "[temt6000x01]")
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
  constexpr units::illuminance::lux_t kExpectedLux(
      2 * kExpectedCurrent.to<float>());
  constexpr units::illuminance::lux_t kExpectedMaxLux(1'000_lx);

  Mock<Adc> mock_adc;
  Fake(Method(mock_adc, GetActiveBits));
  When(Method(mock_adc, Initialize)).AlwaysReturn(Status::kSuccess);
  When(Method(mock_adc, GetActiveBits)).AlwaysReturn(kAdcActiveBits);
  When(Method(mock_adc, Read)).AlwaysReturn(kAdcTestOutput);

  Temt6000x01 light_sensor(
      mock_adc.get(), kReferenceVoltage, kPullDownResistor);

  SECTION("Initialize")
  {
    const bool kIsInitialized = light_sensor.Initialize();

    Verify(Method(mock_adc, Initialize)).Once();
    CHECK(kIsInitialized);
  }

  SECTION("GetIlluminance")
  {
    const units::illuminance::lux_t kLux = light_sensor.GetIlluminance();
    const float kFloatError = fabs(kLux.to<float>() - kExpectedLux.to<float>());

    Verify(Method(mock_adc, Read)).Once();
    CHECK(0.0f <= kFloatError);
    CHECK(kFloatError < 0.001f);
  }

  SECTION("GetMaxIlluminance")
  {
    const units::illuminance::lux_t kMaxLux = light_sensor.GetMaxIlluminance();
    const float kFloatError =
        fabs(kMaxLux.to<float>() - kExpectedMaxLux.to<float>());

    CHECK(0.0f <= kFloatError);
    CHECK(kFloatError < 0.001f);
  }

  SECTION("GetIlluminancePercentage")
  {
    constexpr float kExpectedPercentage =
        (kExpectedLux / kExpectedMaxLux).to<float>();
    const float kPercentage = light_sensor.GetPercentageBrightness();
    const float kFloatError = fabs(kPercentage - kExpectedPercentage);

    Verify(Method(mock_adc, Read)).Once();
    CHECK(0.0f <= kFloatError);
    CHECK(kFloatError < 0.1f);
  }
}
}  // namespace sjsu

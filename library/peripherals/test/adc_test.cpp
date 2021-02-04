#include "peripherals/adc.hpp"

#include "testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing ADC Interface")
{
  Mock<Adc> mock_adc;

  Fake(Method(mock_adc, Read));
  Fake(Method(mock_adc, GetActiveBits));

  Adc & test_subject = mock_adc.get();

  SECTION("GetMaximumValue")
  {
    // Setup
    uint32_t expected_active_bits;

    SECTION("8-bit Resolution")
    {
      expected_active_bits = 8;
    }
    SECTION("10-bit Resolution")
    {
      expected_active_bits = 10;
    }
    SECTION("12-bit Resolution")
    {
      expected_active_bits = 12;
    }

    When(Method(mock_adc, GetActiveBits)).AlwaysReturn(expected_active_bits);

    // Exercise
    uint32_t actual_resolution   = test_subject.GetMaximumValue();
    uint32_t expected_resolution = (1 << expected_active_bits) - 1;

    // Verify
    Verify(Method(mock_adc, GetActiveBits)).Once();
    CHECK(actual_resolution == expected_resolution);
  }

  SECTION("Voltage")
  {
    // Setup
    uint32_t expected_active_bits                          = 12;
    units::voltage::microvolt_t expected_reference_voltage = 3.3_V;

    uint32_t expected_adc_reading;
    units::voltage::microvolt_t expected_voltage;

    SECTION("expected_adc_reading = 0")
    {
      expected_adc_reading = 0;
      expected_voltage     = 0_mV;
    }

    SECTION("expected_adc_reading = 515")
    {
      expected_adc_reading = 515;
      expected_voltage     = 415_mV;
    }

    SECTION("expected_adc_reading = 2000")
    {
      expected_adc_reading = 2000;
      expected_voltage     = 1611_mV;
    }
    SECTION("expected_adc_reading = 4095")
    {
      expected_adc_reading = 4095;
      expected_voltage     = 3300_mV;
    }

    Fake(Method(mock_adc, ModuleInitialize));
    When(Method(mock_adc, Read)).AlwaysReturn(expected_adc_reading);
    When(Method(mock_adc, GetActiveBits)).AlwaysReturn(expected_active_bits);
    // Setup: set settings and initialize mock ADC to save the settings.
    mock_adc.get().settings.reference_voltage = expected_reference_voltage;
    mock_adc.get().Initialize();

    // Exercise
    auto actual_voltage = test_subject.Voltage();

    // Verify
    Verify(Method(mock_adc, GetActiveBits)).Once();
    Verify(Method(mock_adc, Read)).Once();

    CHECK(actual_voltage.to<float>() ==
          doctest::Approx(expected_voltage.to<float>()).epsilon(0.01f));
  }
}
}  // namespace sjsu

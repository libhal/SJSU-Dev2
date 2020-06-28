#include "L1_Peripheral/adc.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "third_party/mockitopp/include/mockitopp/mockitopp.hpp"

namespace sjsu
{
TEST_CASE("Testing ADC Interface")
{
  mockitopp::mock_object<sjsu::Adc> mock_adc;
  sjsu::Adc & test_subject = mock_adc.getInstance();

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

    mock_adc(&sjsu::Adc::GetActiveBits).when().thenReturn(expected_active_bits);

    // Exercise
    uint32_t actual_resolution   = test_subject.GetMaximumValue();
    uint32_t expected_resolution = (1 << expected_active_bits) - 1;

    // Verify
    CHECK(mock_adc(&sjsu::Adc::GetActiveBits).when().exactly(1));
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

    mock_adc(&sjsu::Adc::Read).when().thenReturn(expected_adc_reading);
    mock_adc(&sjsu::Adc::GetActiveBits).when().thenReturn(expected_active_bits);
    mock_adc(&sjsu::Adc::ReferenceVoltage)
        .when()
        .thenReturn(expected_reference_voltage);

    // Exercise
    auto actual_voltage = test_subject.Voltage();

    // Verify
    CHECK(mock_adc(&sjsu::Adc::GetActiveBits).when().exactly(1));
    CHECK(mock_adc(&sjsu::Adc::Read).when().exactly(1));
    CHECK(mock_adc(&sjsu::Adc::ReferenceVoltage).when().exactly(1));

    CHECK(actual_voltage.to<float>() ==
          doctest::Approx(expected_voltage.to<float>()).epsilon(0.01f));
  }
}
}  // namespace sjsu

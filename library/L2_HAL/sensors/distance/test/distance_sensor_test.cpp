// @ingroup SJSU-Dev2
// @defgroup DistanceSensor unit tests
// @brief This file contains the unit tests to validate the DistanceSensor class
// @{
#include "L2_HAL/sensors/distance/distance_sensor.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "utility/log.hpp"
namespace sjsu
{
TEST_CASE("Testing DistanceSensor interface", "[DistanceSensor]")
{
  Mock<DistanceSensor> mock_sensor;
  Fake(Method(mock_sensor, Initialize),
       Method(mock_sensor, GetSignalStrengthPercent));
  DistanceSensor & test = mock_sensor.get();

  SECTION("Check Utility Methods")
  {
    float distance_test;
    constexpr uint32_t kExpectedDistance[3] = {
      500, 0, std::numeric_limits<uint32_t>::max()
    };
    float expected_result_cm[3], expected_result_inch[3], expected_result_ft[3];
    for (int i = 0; i < 3; i++)
    {
      expected_result_cm[i] =
          (kExpectedDistance[i] / DistanceSensor::kConversionCM);
      expected_result_inch[i] =
          (kExpectedDistance[i] / DistanceSensor::kConversionInch);
      expected_result_ft[i] =
          (kExpectedDistance[i] / DistanceSensor::kConversionFt);
    }
    // Test Case: Distance = 500mm
    When(ConstOverloadedMethod(
             mock_sensor, GetDistance, sjsu::Status(uint32_t *)))
        .AlwaysDo([=](uint32_t * distance) {
          *distance = kExpectedDistance[0];
          return sjsu::Status::kSuccess;
        });
    test.GetDistanceCm(&distance_test);
    CHECK(distance_test >= (expected_result_cm[0]) - 0.001f);
    CHECK(distance_test <= (expected_result_cm[0]) + 0.001f);
    test.GetDistanceInch(&distance_test);
    CHECK(distance_test >= (expected_result_inch[0]) - 0.001f);
    CHECK(distance_test <= (expected_result_inch[0]) + 0.001f);
    test.GetDistanceFt(&distance_test);
    CHECK(distance_test >= (expected_result_ft[0]) - 0.001f);
    CHECK(distance_test <= (expected_result_ft[0]) + 0.001f);

    // Test Edge Case: Distance = 0mm
    When(ConstOverloadedMethod(
             mock_sensor, GetDistance, sjsu::Status(uint32_t *)))
        .AlwaysDo([=](uint32_t * distance) {
          *distance = kExpectedDistance[1];
          return sjsu::Status::kSuccess;
        });
    test.GetDistanceCm(&distance_test);
    CHECK(distance_test >= (expected_result_cm[1]) - 0.001f);
    CHECK(distance_test <= (expected_result_cm[1]) + 0.001f);
    test.GetDistanceInch(&distance_test);
    CHECK(distance_test >= (expected_result_inch[1]) - 0.001f);
    CHECK(distance_test <= (expected_result_inch[1]) + 0.001f);
    test.GetDistanceFt(&distance_test);
    CHECK(distance_test >= (expected_result_ft[1]) - 0.001f);
    CHECK(distance_test <= (expected_result_ft[1]) + 0.001f);

    // Test Edge Case: Distance = 0xFFFF_FFFFmm
    When(ConstOverloadedMethod(
             mock_sensor, GetDistance, sjsu::Status(uint32_t *)))
        .AlwaysDo([=](uint32_t * distance) {
          *distance = kExpectedDistance[2];
          return sjsu::Status::kSuccess;
        });
    test.GetDistanceCm(&distance_test);
    CHECK(distance_test >= (expected_result_cm[2]) - 0.001f);
    CHECK(distance_test <= (expected_result_cm[2]) + 0.001f);
    test.GetDistanceInch(&distance_test);
    CHECK(distance_test >= (expected_result_inch[2]) - 0.001f);
    CHECK(distance_test <= (expected_result_inch[2]) + 0.001f);
    test.GetDistanceFt(&distance_test);
    CHECK(distance_test >= (expected_result_ft[2]) - 0.001f);
    CHECK(distance_test <= (expected_result_ft[2]) + 0.001f);
  }
}
}  // namespace sjsu

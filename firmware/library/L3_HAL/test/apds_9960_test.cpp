#include "L3_HAL/apds_9960.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing Gesture Sensor", "[apds_9960]")
{
  Mock<Apds9960> mock;

  // Setup mock behavior
  When(Method(mock, FindDevice)).Return(1);

  Apds9960 &test_subject = mock.get();

  test_subject.FindDevice();

  Verify(Method(mock, FindDevice));
}

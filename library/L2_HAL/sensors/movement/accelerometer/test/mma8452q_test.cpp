#include "L2_HAL/sensors/movement/accelerometer/mma8452q.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Mma8452q);

TEST_CASE("Accelerometer", "[accelerometer]")
{
  Mock<sjsu::I2c> mock_i2c;
  Fake(Method(mock_i2c, Initialize));
  Mma8452q test_subject(mock_i2c.get());

  SECTION("Initialize")
  {
    SECTION("Success")
    {
      // Setup
      When(Method(mock_i2c, Initialize)).AlwaysReturn(Status::kSuccess);

      // Exercise
      auto result = test_subject.Initialize();

      // Verify
      Verify(Method(mock_i2c, Initialize));
      // Verify: Initialize should not have an error
      CHECK(result.has_value());
    }

    SECTION("Failure")
    {
      // Setup
      When(Method(mock_i2c, Initialize)).AlwaysReturn(Status::kNotReadyYet);

      // Exercise
      auto result = test_subject.Initialize();

      // Verify
      Verify(Method(mock_i2c, Initialize));
      // Verify: Initialize should not have an error
      CHECK(!result.has_value());
    }
  }

  // TODO(#1260): Add the rest of the unit tests.
}
}  // namespace sjsu

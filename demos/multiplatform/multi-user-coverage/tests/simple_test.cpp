#include "L4_Testing/testing_frameworks.hpp"

namespace robotics
{
TEST_CASE("Testing robotics simple test", "[robotics-simple-test]")
{
  SECTION("Initialization")
  {
    CHECK(5 == 5);
  }
  SECTION("Conversion and finished conversion")
  {
    CHECK(1 == 1);
  }
}
}  // namespace robotics

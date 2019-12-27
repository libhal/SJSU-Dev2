#include "L4_Testing/testing_frameworks.hpp"
#include "L1_Peripheral/i2c.hpp"

namespace sjsu
{
TEST_CASE("Testing L1 i2c", "[i2c]")
{
  Mock<I2c> i2c;
}
}  // namespace sjsu

#include "L4_Testing/testing_frameworks.hpp"
#include "L1_Peripheral/i2c.hpp"
#include "third_party/mockitopp/include/mockitopp/mockitopp.hpp"

namespace sjsu
{
TEST_CASE("Testing L1 i2c")
{
  mockitopp::mock_object<sjsu::I2c> i2c;
}
}  // namespace sjsu

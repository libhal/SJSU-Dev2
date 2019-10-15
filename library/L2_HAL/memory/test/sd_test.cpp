#include "L2_HAL/memory/sd.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(Sd);

TEST_CASE("Testing SD Card Driver Class", "[sd]")
{
  Mock<Spi> mock_spi;
}
}  // namespace sjsu

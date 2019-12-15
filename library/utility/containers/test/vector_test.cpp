#include "L4_Testing/testing_frameworks.hpp"
#include "utility/containers/vector.hpp"

namespace sjsu
{
TEST_CASE("Testing Vector Container", "[vector-container]")
{
  Vector<int, 128> vector;
}
}  // namespace sjsu

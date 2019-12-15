#include "L4_Testing/testing_frameworks.hpp"
#include "utility/containers/list.hpp"

namespace sjsu
{
TEST_CASE("Testing List Container", "[list-container]")
{
  List<int, 128> list;
}
}  // namespace sjsu

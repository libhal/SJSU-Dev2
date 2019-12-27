#include "L4_Testing/testing_frameworks.hpp"
#include "utility/containers/deque.hpp"

namespace sjsu
{
TEST_CASE("Testing Deque Container", "[deque-container]")
{
  Deque<int, 128> deque;
}
}  // namespace sjsu

#include "L2_HAL/io/parallel_bus.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing ParallelBus Interface")
{
  Mock<ParallelBus> mock_parallel_bus;

  Fake(Method(mock_parallel_bus, SetDirection));

  ParallelBus & test_subject = mock_parallel_bus.get();

  SECTION("SetAsOutput && SetAsInput")
  {
    test_subject.SetAsInput();
    Verify(Method(mock_parallel_bus, SetDirection)
               .Using(sjsu::Gpio::Direction::kInput));

    mock_parallel_bus.ClearInvocationHistory();

    test_subject.SetAsOutput();
    Verify(Method(mock_parallel_bus, SetDirection)
               .Using(sjsu::Gpio::Direction::kOutput));
  }
}
}  // namespace sjsu

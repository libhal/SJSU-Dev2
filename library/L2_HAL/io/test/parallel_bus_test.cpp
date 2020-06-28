#include "L2_HAL/io/parallel_bus.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing ParallelBus Interface")
{
  mockitopp::mock_object<ParallelBus> mock_parallel_bus;

  mock_parallel_bus(&::SetDirection)).when(any<>()).thenReturn();

  ParallelBus & test_subject = mock_parallel_bus.getInstance();

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

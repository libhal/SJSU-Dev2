#include "L2_HAL/sensors/signal/frequency_counter.hpp"

#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing FrequencyCounter")
{
  Mock<HardwareCounter> mock_counter;

  Fake(Method(mock_counter, ModuleInitialize));
  Fake(Method(mock_counter, ModuleEnable));
  Fake(Method(mock_counter, Set));
  Fake(Method(mock_counter, SetDirection));
  Fake(Method(mock_counter, GetCount));

  FrequencyCounter counter(&mock_counter.get());

  SECTION("Initialize()")
  {
    // Setup + Exercise
    counter.ModuleInitialize();

    // Verify
    Verify(Method(mock_counter, ModuleInitialize)).Once();
    Verify(Method(mock_counter, SetDirection)
               .Using(HardwareCounter::Direction::kUp))
        .Once();
  }

  SECTION("Enable()")
  {
    // Setup
    mock_counter.get().SetStateToInitialized();

    // Exercise
    counter.ModuleEnable();

    // Verify
    Verify(Method(mock_counter, ModuleEnable).Using(true)).Once();
  }
}
}  // namespace sjsu

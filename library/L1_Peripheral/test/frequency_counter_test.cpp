#include "L1_Peripheral/frequency_counter.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing FrequencyCounter")
{
  Mock<HardwareCounter> mock_counter;

  Fake(Method(mock_counter, Initialize));
  Fake(Method(mock_counter, Set));
  Fake(Method(mock_counter, SetDirection));
  Fake(Method(mock_counter, Enable));
  Fake(Method(mock_counter, Disable));
  Fake(Method(mock_counter, GetCount));

  FrequencyCounter counter(&mock_counter.get());

  SECTION("Initialize()")
  {
    // Setup + Exercise
    counter.Initialize();

    // Verify
    Verify(Method(mock_counter, Initialize)).Once();
    Verify(Method(mock_counter, SetDirection)
               .Using(HardwareCounter::Direction::kUp))
        .Once();
  }

  SECTION("Disable()")
  {
    // Setup + Exercise
    counter.Disable();

    // Verify
    Verify(Method(mock_counter, Disable)).Once();
  }

  SECTION("Typical Usage Test")
  {
    // Setup
    When(Method(mock_counter, GetCount))
        .Return(500)
        .Return(750)
        .Return(1000)
        .Return(1800)
        .Return(1000);

    // Exercise
    counter.Enable();
    // Exercise: No need to delay, Uptime() is called internally which will
    // increment the uptime by 1us.
    //
    // Exercise: Will pull 500 and 750 from GetCount()
    auto actual_frequency = counter.GetFrequency();

    SECTION("Normal Usage")
    {
      // Verify
      // Verify: (500 ticks / 1us) => 500MHz
      CHECK(actual_frequency == 500_MHz);
    }

    SECTION("Using Reset()")
    {
      counter.Reset();
      // Exercise: Total Delta during GetFrequency will be 3us + 1us
      sjsu::Delay(3us);
      // Exercise: Will pull 750 and 1000 from GetCount()
      auto new_actual_frequency = counter.GetFrequency();

      // Verify
      // Verify: (800 ticks / 4us) => 200MHz
      CHECK(new_actual_frequency == 200_MHz);
    }

    Verify(Method(mock_counter, Enable)).Once();
  }
}
}  // namespace sjsu

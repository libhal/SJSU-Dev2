#include "L1_Peripheral/frequency_counter.hpp"
#include "L4_Testing/testing_frameworks.hpp"
#include "third_party/mockitopp/include/mockitopp/mockitopp.hpp"

namespace sjsu
{
TEST_CASE("Testing FrequencyCounter")
{
  using mockitopp::matcher::any;
  mockitopp::mock_object<HardwareCounter> mock_counter;

  FrequencyCounter counter(&mock_counter.getInstance());

  SECTION("Initialize()")
  {
    // Setup
    mock_counter(&HardwareCounter::Initialize).when().thenReturn();
    mock_counter(&HardwareCounter::SetDirection)
        .when(any<HardwareCounter::Direction>())
        .thenReturn();

    // Exercise
    counter.Initialize();

    // Verify
    CHECK(mock_counter(&HardwareCounter::Initialize).when().exactly(1));
    CHECK(mock_counter(&HardwareCounter::SetDirection)
              .when(HardwareCounter::Direction::kUp)
              .exactly(1));
  }

  SECTION("Disable()")
  {
    // Setup
    mock_counter(&HardwareCounter::Disable).when().thenReturn();

    // Exercise
    counter.Disable();

    // Verify
    CHECK(mock_counter(&HardwareCounter::Disable).when().exactly(1));
  }

  SECTION("Typical Usage Test")
  {
    // Setup
    mock_counter(&HardwareCounter::Enable).when().thenReturn();

    mock_counter(&HardwareCounter::GetCount)
        .when()
        .thenReturn(500)
        .thenReturn(750)
        .thenReturn(1000)
        .thenReturn(1800)
        .thenReturn(1000);

    // Exercise
    counter.Enable();

    // Exercise: No need to delay, Uptime() is called internally which will
    //           increment the uptime by 1us.
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

    CHECK(mock_counter(&HardwareCounter::Enable).when().exactly(1));
  }
}
}  // namespace sjsu

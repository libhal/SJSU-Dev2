#include "peripherals/pin.hpp"

#include "testing/testing_frameworks.hpp"

namespace sjsu
{
TEST_CASE("Testing L1 pin")
{
  Mock<sjsu::Pin> mock_pin;
  sjsu::Pin & pin = mock_pin.get();

  SECTION("PullUp()")
  {
    // Exercise
    pin.settings.PullUp();

    // Verify
    CHECK(pin.settings.resistor == PinSettings_t::Resistor::kPullUp);
  }
  SECTION("PullDown()")
  {
    // Exercise
    pin.settings.PullDown();

    // Verify
    CHECK(pin.settings.resistor == PinSettings_t::Resistor::kPullDown);
  }
  SECTION("Floating()")
  {
    // Exercise
    pin.settings.Floating();

    // Verify
    CHECK(pin.settings.resistor == PinSettings_t::Resistor::kNone);
  }
}
}  // namespace sjsu

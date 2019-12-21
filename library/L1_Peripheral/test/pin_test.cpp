#include "L4_Testing/testing_frameworks.hpp"
#include "L1_Peripheral/pin.hpp"

namespace sjsu
{
TEST_CASE("Testing L1 pin", "[pin]")
{
  Mock<sjsu::Pin> mock_pin;
  Fake(Method(mock_pin, SetPull));

  sjsu::Pin & pin = mock_pin.get();

  SECTION("PullUp()")
  {
    // Exercise
    pin.PullUp();

    // Verify
    Verify(Method(mock_pin, SetPull).Using(sjsu::Pin::Resistor::kPullUp));
  }
  SECTION("PullDown()")
  {
    // Exercise
    pin.PullDown();

    // Verify
    Verify(Method(mock_pin, SetPull).Using(sjsu::Pin::Resistor::kPullDown));
  }
  SECTION("SetFloating()")
  {
    // Exercise
    pin.SetFloating();

    // Verify
    Verify(Method(mock_pin, SetPull).Using(sjsu::Pin::Resistor::kNone));
  }
}
}  // namespace sjsu

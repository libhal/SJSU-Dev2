#include "L4_Testing/testing_frameworks.hpp"
#include "L1_Peripheral/pin.hpp"

namespace sjsu
{
TEST_CASE("Testing L1 pin")
{
  mockitopp::mock_object<sjsu::Pin> mock_pin;
  mock_pin(&::SetPull)).when(any<>()).thenReturn();

  sjsu::Pin & pin = mock_pin.getInstance();

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

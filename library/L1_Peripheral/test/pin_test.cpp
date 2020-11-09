#include "L4_Testing/testing_frameworks.hpp"
#include "L1_Peripheral/pin.hpp"

namespace sjsu
{
TEST_CASE("Testing L1 pin")
{
  Mock<sjsu::Pin> mock_pin;
  Fake(Method(mock_pin, ConfigurePullResistor));

  sjsu::Pin & pin = mock_pin.get();

  SECTION("PullUp()")
  {
    // Exercise
    pin.ConfigurePullUp();

    // Verify
    Verify(Method(mock_pin, ConfigurePullResistor)
               .Using(sjsu::Pin::Resistor::kPullUp));
  }
  SECTION("PullDown()")
  {
    // Exercise
    pin.ConfigurePullDown();

    // Verify
    Verify(Method(mock_pin, ConfigurePullResistor)
               .Using(sjsu::Pin::Resistor::kPullDown));
  }
  SECTION("ConfigureFloating()")
  {
    // Exercise
    pin.ConfigureFloating();

    // Verify
    Verify(Method(mock_pin, ConfigurePullResistor)
               .Using(sjsu::Pin::Resistor::kNone));
  }
}
}  // namespace sjsu

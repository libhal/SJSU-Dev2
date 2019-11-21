#include "L1_Peripheral/hardware_counter.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
// Helps with test coverage. Every object should be wrapped in the
// EMIT_ALL_METHODS macro.
EMIT_ALL_METHODS(GpioCounter);

TEST_CASE("Testing L1 GpioCounter", "[gpio-counter]")
{
  Mock<sjsu::Pin> mock_pin;
  Fake(Method(mock_pin, SetPull));

  // Create mocked versions of the sjsu::Pin
  Mock<sjsu::Gpio> mock_gpio;
  Fake(Method(mock_gpio, SetDirection));
  Fake(Method(mock_gpio, AttachInterrupt));
  Fake(Method(mock_gpio, DetachInterrupt));
  When(Method(mock_gpio, GetPin)).AlwaysReturn(mock_pin.get());

  SECTION("Initialize() with rising edge and default pull up resistor")
  {
    // Setup
    GpioCounter test_subject(mock_gpio.get(), sjsu::Gpio::Edge::kEdgeBoth);
    // Execute
    test_subject.Initialize();
    // Verify
    Verify(Method(mock_gpio, SetDirection)
               .Using(sjsu::Gpio::Direction::kInput));
    Verify(Method(mock_pin, SetPull)
               .Using(sjsu::Pin::Resistor::kPullUp));
  }
}
}  // namespace sjsu

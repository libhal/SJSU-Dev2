#include "L0_LowLevel/LPC40xx.h"
#include "L2_HAL/switches/button.hpp"
#include "L4_Testing/testing_frameworks.hpp"

EMIT_ALL_METHODS(Button);

TEST_CASE("Testing Button", "[button]")
{
  // Make a mock pin to work with
  Mock<PinInterface> mock_pin;
  // Retrieve a reference to the PinInterface to be injected as the return value
  // of GpioInterfaces's GetPin() method.
  PinInterface & test_pin = mock_pin.get();
  // Fake the implementation of SetAsActiveLow and SetMode to be inspected later
  Fake(Method(mock_pin, SetAsActiveLow), Method(mock_pin, SetMode));

  // Create a mock gpio object
  Mock<GpioInterface> mock_gpio;
  // Fake Read and SetAsInput so we can inspect them later
  Fake(Method(mock_gpio, Read), Method(mock_gpio, SetAsInput));
  // Fake implementation of GetPin() to return our mock_pin reference. So when
  // Button retrieves test_pin from GetPin() it will run our faked
  // SetAsActiveLow and SetMode methods, allowing us to change them later.
  When(Method(mock_gpio, GetPin)).AlwaysReturn(test_pin);

  // Retrieve GpioInterface reference ot be passed to the test subject
  GpioInterface & test_gpio = mock_gpio.get();

  // Inject test_gpio into button object
  Button test_subject(&test_gpio);

  SECTION("Initialize")
  {
    test_subject.Initialize();
    Verify(Method(mock_gpio, SetAsInput),
           Method(mock_pin, SetMode).Using(PinInterface::Mode::kPullDown),
           Method(mock_pin, SetAsActiveLow));
  }
  SECTION("Invert Button Signal")
  {
    test_subject.InvertButtonSignal(true);
    Verify(Method(mock_pin, SetAsActiveLow).Using(true));
    test_subject.InvertButtonSignal(false);
    Verify(Method(mock_pin, SetAsActiveLow).Using(false));
  }
  SECTION("Button Released")
  {
    // Reset button state
    test_subject.ResetState();
    // Simulate button being idle
    When(Method(mock_gpio, Read)).AlwaysReturn(false);
    // With this check, the state of the button should be false, and since we
    // have not run the Released() method yet, the Released() method should
    // return false.
    CHECK(!test_subject.Released());
    // Simulate button being pressed
    When(Method(mock_gpio, Read)).AlwaysReturn(true);
    // Button is currently pressed but has not been released, so this method
    // should return false again.
    CHECK(!test_subject.Released());
    // Simulate button being released
    When(Method(mock_gpio, Read)).AlwaysReturn(false);
    // Button has moved from a pressed to released state, so this method should
    // return true.
    CHECK(test_subject.Released());
  }
  SECTION("Button Pressed")
  {
    // Reset button state
    test_subject.ResetState();
    // Simulate button having already been pressed
    When(Method(mock_gpio, Read)).AlwaysReturn(true);
    // With this check, the state of the button should be true, and since we
    // have not run the Pressed() method yet, the Press() method should
    // return false.
    CHECK(!test_subject.Pressed());
    // Simulate button being released
    When(Method(mock_gpio, Read)).AlwaysReturn(false);
    // Button is currently released but has not been pressed, so this method
    // should return false again.
    CHECK(!test_subject.Pressed());
    // Simulate button being pressed
    When(Method(mock_gpio, Read)).AlwaysReturn(true);
    // Button has changed from a low to high state signaling a pressed event,
    // return true.
    CHECK(test_subject.Pressed());
  }
}

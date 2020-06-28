#include "L4_Testing/testing_frameworks.hpp"
#include "L1_Peripheral/gpio.hpp"
#include "third_party/mockitopp/include/mockitopp/mockitopp.hpp"

namespace sjsu
{
TEST_CASE("Testing L1 gpio")
{
  using mockitopp::matcher::any;
  mockitopp::mock_object<sjsu::Gpio> mock_gpio;

  mock_gpio(&sjsu::Gpio::SetDirection)
      .when(any<sjsu::Gpio::Direction>())
      .thenReturn();
  mock_gpio(&sjsu::Gpio::Set).when(any<sjsu::Gpio::State>()).thenReturn();

  sjsu::Gpio & gpio = mock_gpio.getInstance();

  SECTION("SetAsInput()")
  {
    // Exercise
    gpio.SetAsInput();

    // Verify
    CHECK(mock_gpio(&sjsu::Gpio::SetDirection)
              .when(sjsu::Gpio::Direction::kInput)
              .exactly(1));
  }

  SECTION("SetAsOutput()")
  {
    // Exercise
    gpio.SetAsOutput();

    // Verify
    CHECK(mock_gpio(&sjsu::Gpio::SetDirection)
              .when(sjsu::Gpio::Direction::kOutput)
              .exactly(1));
  }

  SECTION("SetHigh()")
  {
    // Exercise
    gpio.SetHigh();

    // Verify
    CHECK(
        mock_gpio(&sjsu::Gpio::Set).when(sjsu::Gpio::State::kHigh).exactly(1));
  }

  SECTION("SetLow()")
  {
    // Exercise
    gpio.SetLow();

    // Verify
    CHECK(mock_gpio(&sjsu::Gpio::Set).when(sjsu::Gpio::State::kLow).exactly(1));
  }
}
}  // namespace sjsu

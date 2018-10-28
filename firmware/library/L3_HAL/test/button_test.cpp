#include "L0_LowLevel/LPC40xx.h"
#include "L3_HAL/button.hpp"
#include "L5_Testing/testing_frameworks.hpp"

TEST_CASE("Testing Button", "[button]")
{
  // Simulated local version of LPC_IOCON
  // manipulation by side effect of Pin method calls
  LPC_IOCON_TypeDef local_iocon;
  // Substitute the memory mapped LPC_IOCON
  // with the local_iocon test struture
  memset(&local_iocon, 0, sizeof(local_iocon));

  // Redirects manipulation to the 'local_iocon'
  Pin::pin_map = reinterpret_cast<Pin::PinMap_t *>(&local_iocon);
  LPC_GPIO_TypeDef local_gpio_port[2];
  memset(&local_gpio_port, 0, sizeof(local_gpio_port));

  // Only Port 1 is tested in this test.
  Gpio::gpio_port[1] = &local_gpio_port[1];
  // Onboard button of Port 1 and Pin 9 will be used for this test
  Button test_subject(1, 9);

  SECTION("Initialize")
  {
    constexpr uint8_t kInputSet     = 0b0;
    constexpr uint8_t kPin9         = 9;
    constexpr uint8_t kModePosition = 3;
    constexpr uint32_t kMask        = 0b11 << kModePosition;

    constexpr uint32_t kExpectedForPullDown =
        static_cast<uint8_t>(PinInterface::Mode::kPullDown) << kModePosition;

    test_subject.Initialize();

    CHECK(kExpectedForPullDown == (local_iocon.P1_9 & kMask));
    CHECK(((local_gpio_port[1].DIR >> kPin9) & 1) == kInputSet);
  }
  SECTION("Button Released")
  {
    constexpr bool kExpectedResult = true;
    constexpr uint8_t kPin9        = 9;
    // Reset Button's State
    test_subject.ResetState();
    // Set P1_9 for button read == true
    local_gpio_port[1].PIN |= (1 << kPin9);
    // Simulate button being pressed.
    test_subject.Released();
    // was_pressed is now true.
    // Now clear P1_9 for button read == false
    local_gpio_port[1].PIN &= ~(1 << kPin9);
    // Button is now released.
    // Check if button released condition is true
    CHECK(test_subject.Released() == kExpectedResult);
  }
  SECTION("Button Pressed")
  {
    constexpr bool kExpectedResult = true;
    constexpr uint8_t kPin9        = 9;
    // Reset Button State
    test_subject.ResetState();
    // Simulate button being pressed.
    // Set P1_9 for button read == true
    local_gpio_port[1].PIN |= (1 << kPin9);
    // Check if button pressed condition is true
    CHECK(test_subject.Pressed() == kExpectedResult);
  }
  SECTION("Invert Button Signal")
  {
    constexpr uint8_t kInvertPosition = 6;
    constexpr uint32_t kMask          = 0b1 << kInvertPosition;
    // Source: "UM10562 LPC408x/407x User manual" table 83 page 132
    // Check that mapped pin P1.9's input inversion bit is set to 1
    test_subject.InvertButtonSignal(true);
    CHECK(kMask == (local_iocon.P1_9 & kMask));
    // Check that mapped pin P1.9's input inversion bit is set to 0
    test_subject.InvertButtonSignal(false);
    CHECK(0 == (local_iocon.P1_9 & kMask));
  }
  Gpio::gpio_port[0] = LPC_GPIO0;
  Gpio::gpio_port[1] = LPC_GPIO1;
  Gpio::gpio_port[2] = LPC_GPIO2;
  Gpio::gpio_port[3] = LPC_GPIO3;
  Gpio::gpio_port[4] = LPC_GPIO4;
  Gpio::gpio_port[5] = LPC_GPIO5;
  Pin::pin_map       = reinterpret_cast<Pin::PinMap_t *>(LPC_IOCON);
}

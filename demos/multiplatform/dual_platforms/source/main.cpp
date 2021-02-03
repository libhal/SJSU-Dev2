#include "peripherals/gpio.hpp"
#include "peripherals/inactive.hpp"
#include "peripherals/lpc17xx/gpio.hpp"
#include "peripherals/lpc40xx/gpio.hpp"
#include "devices/switches/button.hpp"
#include "utility/build_info.hpp"
#include "utility/time/time.hpp"

int main()
{
  sjsu::LogInfo("Dual Platform Application Starting...");
  sjsu::LogInfo(
      "This demo creates a Button and LED that uses a GPIO driver form "
      "different devices...");

  // Phase #1:
  // Define all of the peripheral you plan to use as pointers. Pointers must be
  // used in order to do the next step
  sjsu::Gpio * button_gpio = &sjsu::GetInactive<sjsu::Gpio>();
  sjsu::Gpio * led_gpio    = &sjsu::GetInactive<sjsu::Gpio>();

  // Phase #2:
  // Use "constexpr if" to instantiate the peripherals for each platform
  if constexpr (sjsu::build::kPlatform == sjsu::build::Platform::lpc17xx)
  {
    sjsu::LogInfo("Current Platform LPC17xx...");
    // If a hardware driver object needs to be referenced by an interface
    // outside the scope of this "constexpr if" statement, make it static
    // to allocate it statically, otherwise the objects created in this
    // block will be destructed after the program leaves this scope.
    static sjsu::lpc17xx::Gpio lpc17xx_button_gpio(1, 9);
    static sjsu::lpc17xx::Gpio lpc17xx_led_gpio(1, 0);
    button_gpio = &lpc17xx_button_gpio;
    led_gpio    = &lpc17xx_led_gpio;
  }
  else if constexpr (sjsu::build::kPlatform == sjsu::build::Platform::lpc40xx)
  {
    sjsu::LogInfo("Current Platform LPC40xx...");
    static sjsu::lpc40xx::Gpio lpc40xx_button_gpio(0, 29);
    static sjsu::lpc40xx::Gpio lpc40xx_led_gpio(1, 18);
    button_gpio = &lpc40xx_button_gpio;
    led_gpio    = &lpc40xx_led_gpio;
  }
  else
  {
    sjsu::LogError("Invalid platform for this application!");
    return -1;
  }

  // Phase #3:
  // Pass and construct all devices abstraction objects. Typically the hardware
  // abstractions are platform independent, thus passing them here will cause
  // implementation details of a particular platform to leak into this
  // application.
  sjsu::LogInfo("Constructing HAL object (in this case button class)...");
  sjsu::Button button(*button_gpio);

  // Phase #4:
  // Now you can use the devices abstractions and peripherals interfaces as
  // you wish.

  // Also note that not all of this has to happen in the main function. These
  // can be seperated into other functions or even their own classes. This is
  // just showing you a very small sequence of steps that can achieve the
  // desired effect of building a system that can port between platforms.
  sjsu::LogInfo("Initialize...");
  button.Initialize();
  led_gpio->Initialize();

  sjsu::LogInfo("Use...");
  led_gpio->SetAsOutput();

  sjsu::LogInfo("Press and Release the button to toggle the LED...");
  while (true)
  {
    if (button.Released())
    {
      led_gpio->Toggle();
    }
    sjsu::Delay(100ms);
  }
  return 0;
}

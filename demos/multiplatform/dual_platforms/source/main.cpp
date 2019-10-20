#include "L1_Peripheral/gpio.hpp"
#include "L1_Peripheral/inactive.hpp"
#include "L1_Peripheral/lpc17xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L2_HAL/switches/button.hpp"
#include "utility/time.hpp"
#include "utility/build_info.hpp"

int main()
{
  // Phase #1:
  // Define all of the peripheral you plan to use as pointers. Pointers must be
  // used in order to do the next step
  const sjsu::Gpio * button_gpio;
  const sjsu::Gpio * led_gpio;
  // Phase #2:
  // Use "constexpr if" to instantiate the peripherals for each platform
  if constexpr (sjsu::build::kPlatform == sjsu::build::Platform::lpc17xx)
  {
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
    static sjsu::lpc40xx::Gpio lpc40xx_button_gpio(1, 19);
    static sjsu::lpc40xx::Gpio lpc40xx_led_gpio(2, 3);
    button_gpio = &lpc40xx_button_gpio;
    led_gpio    = &lpc40xx_led_gpio;
  }
  else
  {
    LOG_ERROR("Invalid platform for this application!");
    sjsu::Halt();
  }
  // Phase #3:
  // Pass and initialize all L2 HAL abstraction objects. Typically the hardware
  // abstractions are platform independent, thus passing them here will cause
  // implementation details of a particular platform to leak into this
  // application.
  sjsu::Button button(*button_gpio);

  // DONE!
  // Now you can use the L2 HAL abstractions and L1 Peripheral interfaces as
  // you wish.

  // Also note that not all of this has to happen in the main function. These
  // can be seperated into other functions or even their own classes. This is
  // just showing you a very small sequence of steps that can achieve the
  // desired effect of building a system that can port between platforms.
  button.Initialize();
  led_gpio->SetAsOutput();

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

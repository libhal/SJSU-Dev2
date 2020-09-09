#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/hardware_counter.hpp"
#include "L2_HAL/boards/sjtwo.hpp"
#include "utility/time.hpp"
#include "utility/log.hpp"

int main()
{
  sjsu::LogInfo("Gpio Hardware Counter Application Starting...");
  // Create a GPIO to read from. This can be any GPIO that supports GPIO
  // interrupts. On the LPC40xx, that would be pins on port 0 and port 2.
  //
  // Feel free to change this to some other gpio port and pin number.
  sjsu::lpc40xx::Gpio gpio(0, 0);
  // Pass the GPIO above into the gpio counter to be controlled by it.
  // The second parameter allows you to change which event triggers a count.
  // In this case, we want to trigger on rising and falling edges of the pin.
  // Note that this
  sjsu::GpioCounter counter(gpio, sjsu::Gpio::Edge::kRising);
  // Required: Initialize the hardware.
  counter.Initialize();
  // Required: Enable the counter.
  counter.Enable();

  sjsu::LogInfo(
      "With every rising edge of pin P%u.%u, the counter will increase and its "
      "value will be printed to stdout.",
      gpio.GetPin().GetPort(), gpio.GetPin().GetPin());

  int32_t previous_count = 0;

  while (true)
  {
    int32_t current_count = SJ2_RETURN_VALUE_ON_ERROR(counter.GetCount(), -1);
    // Anytime the count changes, we print out the latest count.
    if (previous_count != current_count)
    {
      sjsu::LogInfo("Hardare Count %" PRId32 "\n", current_count);
      previous_count = current_count;
    }
    // This throttles how often we print as well as demonstrates that the
    // counter still counts even when the processor is stuck in a busy loop.
    // This is due to the fact that sjsu::HardwareCounters must be able to work
    // in a asynchronous manner, such as utilizing interrupts, internal
    // peripheral counters, or external counters.
    sjsu::Delay(1s);
  }
  return 0;
}

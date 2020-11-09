#include "L1_Peripheral/hardware_counter.hpp"
#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L2_HAL/sensors/battery/ltc4150.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"

int main()
{
  sjsu::LogInfo("Ltc4150 application starting...");

  sjsu::LogInfo("Connect LTC4150 INT signal to the P2[0] pin.");
  sjsu::LogInfo(
      "Everytime there is an interrupt the demo will display the current mAh "
      "that have passed through the sense resistor.");

  // Creating GPIO on pin 2.0
  sjsu::lpc40xx::Gpio tick_pin(2, 0);

  // Pass the tick_pin GPIO to the gpio counter. We want to keep
  // track of a tick for a falling edge interrupt from the tick_pin.
  sjsu::GpioCounter tick_counter(tick_pin, sjsu::Gpio::Edge::kFalling);

  // Creating GPIO on pin 2.1
  sjsu::lpc40xx::Gpio polarity_pin(2, 1);

  // Set the sense resistor resistance to be 50 milliohms. Change this to
  // whatever your application is using.
  units::impedance::milliohm_t resistance = 50_mOhm;

  // Initialize the LTC4150 with the two GPIO pins and resistance.
  sjsu::Ltc4150 counter(tick_counter, polarity_pin, resistance);

  // Initialize the LTC4150 class, setting two GPIO pins to be outputs and
  // attaching interrupts.
  counter.Initialize();

  // Enable counter
  counter.Enable();

  while (true)
  {
    // Grab the calculated charge from the counter, based on how many ticks it
    // received and the polarity.
    units::charge::milliampere_hour_t recent_charge = counter.GetCharge();

    sjsu::LogInfo("Current Charge: %f mAh", recent_charge.to<double>());

    // This limits how often we print as well as demonstrates that the
    // LTC4150 still counts even when the processor is stuck in a busy loop.
    // This is due to the fact that the LTC4150 uses interrupts on the supplied
    // GPIO pins to track tick count and polarity.
    sjsu::Delay(1s);
  }

  return 0;
}

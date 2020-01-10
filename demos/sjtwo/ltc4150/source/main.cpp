#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L2_HAL/sensors/battery/ltc4150.hpp"
#include "L1_Peripheral/hardware_counter.hpp"
#include "utility/log.hpp"
#include "utility/time.hpp"
#include "utility/units.hpp"

bool ApproxEquality(float expected, float actual, float resolution)
{
  return -resolution < (actual - expected) && (actual - expected) < resolution;
}

int main()
{
  constexpr float kResolution = 0.001f;
  LOG_INFO("Ltc4150 application starting...");

  // Creating GPIO on pin 2.0
  sjsu::lpc40xx::Gpio tick_pin(2, 0);

  // Pass the tick_pin GPIO to the gpio counter. We want to keep
  // track of a tick for a falling edge interrupt from the tick_pin.
  sjsu::GpioCounter tick_counter(tick_pin, sjsu::Gpio::Edge::kEdgeFalling);

  // Creating GPIO on pin 2.1
  sjsu::lpc40xx::Gpio polarity_pin(2, 1);

  // Set the sense resistor resistance to be 50 milliohms.
  units::impedance::milliohm_t resistance = 50_mOhm;

  // Initialize the LTC4150 with the two GPIO pins and resistance.
  sjsu::Ltc4150 counter(tick_counter, polarity_pin, resistance);

  // Initialize the LTC4150 class, setting two GPIO pins to be outputs and
  // attaching interrupts.
  counter.Initialize();

  // Set the previous charge to be 0 mAh.
  units::charge::milliampere_hour_t previous_charge = 0_mAh;

  while (true)
  {
    // Grab the calculated charge from the counter, based on how many ticks it
    // received and the polarity.
    units::charge::milliampere_hour_t recent_charge = counter.GetCharge();

    // Anytime the charge changes, we print out the latest count.
    if (!ApproxEquality(previous_charge.to<float>(),
                        recent_charge.to<float>(),
                        kResolution))
    {
      LOG_INFO("Current Charge: %f mAh", recent_charge.to<double>());
      previous_charge = recent_charge;
    }
    // This limits how often we print as well as demonstrates that the
    // LTC4150 still counts even when the processor is stuck in a busy loop.
    // This is due to the fact that the LTC4150 uses interrupts on the supplied
    // GPIO pins to track tick count and polarity.
    sjsu::Delay(1s);
  }

  return 0;
}

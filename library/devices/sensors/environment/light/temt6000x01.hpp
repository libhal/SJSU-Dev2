#pragma once

#include "devices/sensors/environment/light_sensor.hpp"

#include "peripherals/adc.hpp"
#include "utility/log.hpp"

namespace sjsu
{
/// The TEMT6000X01 is an ambient light sensor that provides light readings
/// that can be read using the ADC peripheral.
class Temt6000x01 final : public LightSensor
{
 public:
  /// @param adc The ADC peripheral used to read the sensor data.
  /// @param pull_down_resistance The resistance of the pull down resistor used
  ///                             in the circuit.
  explicit constexpr Temt6000x01(sjsu::Adc & adc,
                                 units::impedance::ohm_t pull_down_resistance)
      : adc_(adc), kPullDownResistance(pull_down_resistance)
  {
  }

  /// Initializes the ADC driver.
  ///
  /// @returns The initialization status.
  void ModuleInitialize() override
  {
    adc_.Initialize();
  }

  /// @returns The illuminance in units of lux ranging from 1 - 1'000.
  units::illuminance::lux_t GetIlluminance() override
  {
    // current = voltage / resistance
    const units::current::microampere_t kCurrent =
        (adc_.Voltage() / kPullDownResistance);
    // From Fig. 3 - Photo Current vs. Illuminance of TEMT6000X01 datasheet,
    // Solve for x using log(y) = m * log(x) + b, where x is the
    // illuminance in lux and y is the current in micro-amps.
    //
    // Plugging in (20, 10) and (100, 50) to solve for m:
    // m = ∆log(y) / ∆log(x)
    //   = log(y2 / y1) / log(x2 / x1)
    //   = log(50 / 10) / log(100 / 20)
    //   = log(5) / log(5)
    // m = 1
    //
    // Plugging in (100, 50) to solve for b:
    // log(y) = log(x) + b
    // log(50) = log(100) + b
    // b = -log(2)
    //
    // Solve for x:
    // log(y) = log(x) - log(2)
    //        = log(x / 2)
    // --> y = x / 2
    // --> x = 2 * y
    return units::illuminance::lux_t(2.0f * kCurrent.to<float>());
  }

  /// @returns The maximum illuminance that can be handled by the sensor.  The
  ///          device has an illumination range of 1 - 1000 lux.
  units::illuminance::lux_t GetMaxIlluminance() override
  {
    return 1'000_lx;
  }

 private:
  /// The ADC peripheral used to capture the input illuminance data.
  sjsu::Adc & adc_;
  /// Resistance of the pull down resistor.
  const units::impedance::ohm_t kPullDownResistance;
};
}  // namespace sjsu

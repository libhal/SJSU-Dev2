#pragma once

#include "L2_HAL/sensors/environment/light_sensor.hpp"

#include "L1_Peripheral/adc.hpp"
#include "utility/log.hpp"

namespace sjsu
{
class Temt6000x01 final : public LightSensor
{
 public:
  explicit constexpr Temt6000x01(Adc & adc,
                                 units::voltage::volt_t adc_reference_voltage,
                                 units::impedance::ohm_t pull_down_resistance)
      : adc_(adc),
        kAdcReferenceVoltage(adc_reference_voltage),
        kPullDownResistance(pull_down_resistance),
        adc_resolution_(0)
  {
    adc_resolution_ = powf(2, adc_.GetActiveBits()) - 1;
  }

  bool Initialize() const override
  {
    SJ2_ASSERT_FATAL(adc_.Initialize() == Status::kSuccess,
                     "Failed to initalize ADC channel.");
    return true;
  }
  /// @returns The illuminance in units of lux ranging from 1 - 1'000.
  units::illuminance::lux_t GetIlluminance() const override
  {
    // voltage = adc_output * (adc_reference_voltage / adc_resolution)
    const units::voltage::microvolt_t kVoltage(
        adc_.Read() * kAdcReferenceVoltage / adc_resolution_);
    // current = voltage / resistance
    const units::current::microampere_t kCurrent =
        (kVoltage / kPullDownResistance);
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
    return units::illuminance::lux_t(2 * kCurrent.to<float>());
  }
  /// @returns The illuminance percentage ranging from 0.0f to 1.0f.
  float GetPercentageBrightness() const override
  {
    return (GetIlluminance() / GetMaxIlluminance()).to<float>();
  }
  /// @returns The maximum illuminance that can be handled by the sensor.
  ///          The device has an illumination range of 1 - 1000 lux.
  units::illuminance::lux_t GetMaxIlluminance() const override
  {
    return 1'000_lx;
  }

 private:
  const Adc & adc_;
  const units::voltage::microvolt_t kAdcReferenceVoltage;
  const units::impedance::ohm_t kPullDownResistance;
  /// This value varies base on the number of bits supported by the ADC.
  /// The resoltuion is determined by: (2^(# of bits)) - 1.
  float adc_resolution_;
};
}  // namespace sjsu

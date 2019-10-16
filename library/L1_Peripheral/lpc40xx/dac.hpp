#pragma once

#include "L1_Peripheral/dac.hpp"

#include "L0_Platform/lpc40xx/LPC40xx.h"
#include "L1_Peripheral/lpc40xx/pin.hpp"
#include "L1_Peripheral/lpc40xx/system_controller.hpp"
#include "utility/log.hpp"
#include "utility/status.hpp"

namespace sjsu
{
namespace lpc40xx
{
/// Implementation of digital-to-analog converter for lpc40xx.
class Dac final : public sjsu::Dac
{
 public:
  /// Definitions for DAC bias setting. Setting this to high will use more
  /// power, but will have a faster refresh rate.
  enum class Bias : bool
  {
    kHigh = 0,
    kLow  = 1
  };
  /// DAC control register bitmasks.
  struct Control  // NOLINT
  {
    /// This bit field holds the output value of the DAC. This bit field is also
    /// the exact bit resolution of the DAC output.
    static constexpr bit::Mask kValue = bit::CreateMaskFromRange(6, 15);
    /// Bias bit position in control register. See Bias enumeration for details
    /// about how this bit works.
    static constexpr bit::Mask kBias = bit::CreateMaskFromRange(16);
  };
  /// The only DAC output pin on the lpc40xx.
  static constexpr sjsu::lpc40xx::Pin kDacPin = Pin::CreatePin<0, 26>();
  /// Voltage reference for the lpc40xx voltage.
  static constexpr float kVref = 3.3f;
  /// Maximum numeric value of the ADC value register.
  static constexpr uint32_t kMaximumValue = (1 << Control::kValue.width) - 1;
  /// Pointer to the LPC DAC peripheral in memory
  inline static LPC_DAC_TypeDef * dac_register = LPC_DAC;

  /// Construct lpc40xx Dac object.
  ///
  /// @param pin - defaults to the only dac pin on the board. The only reason to
  ///        use this parameter would be for unit testing. Otherwise, it should
  ///        not be changed from its default.
  explicit constexpr Dac(const sjsu::Pin & pin = kDacPin) : dac_pin_(pin) {}
  /// Initialize DAC hardware, enable dac Pin, initial Bias level set to 0.
  Status Initialize() const override
  {
    static constexpr uint8_t kDacMode = 0b010;
    dac_pin_.SetPinFunction(kDacMode);
    // Temporarily convert dac_pin to a lpc40xx::Pin so we can use the
    // EnableDacs() method featured in the LPC40xx pin object.
    // The program is ill-formed if the pin's implementation was not a lpc40xx
    // pin.
    const sjsu::lpc40xx::Pin & lpc40xx_dac_pin =
        reinterpret_cast<const sjsu::lpc40xx::Pin &>(dac_pin_);
    lpc40xx_dac_pin.EnableDac();
    dac_pin_.SetAsAnalogMode();
    dac_pin_.SetPull(Pin::Resistor::kNone);
    // Disable interrupt and DMA
    dac_register->CTRL = 0;
    // Set Update Rate to 1MHz
    SetBias(Bias::kHigh);

    return Status::kSuccess;
  }
  void Write(uint32_t dac_output) const override
  {
    // The DAC output is a 10 bit input and thus it is necessary to
    // ensure dac_output is less than 1024 (largest 10-bit number)
    SJ2_ASSERT_FATAL(dac_output < kMaximumValue,
                     "DAC output set above 1023. Must be between 0-1023.");
    dac_register->CR =
        bit::Insert(dac_register->CR, dac_output, Control::kValue);
  }
  void SetVoltage(float voltage) const override
  {
    float value         = (voltage * kMaximumValue) / kVref;
    uint32_t conversion = static_cast<uint32_t>(value);
    SJ2_ASSERT_FATAL(
        voltage < kVref,
        "DAC output was set above 3.3V. Must be between 0V and 3.3V.");
    Write(conversion);
  }
  /// Sets the Bias for the Dac, which determines the settling time, max
  /// current, and the allowed maximum update rate
  void SetBias(Bias bias_level) const
  {
    bool bias        = static_cast<bool>(bias_level);
    dac_register->CR = bit::Insert(dac_register->CR, bias, Control::kBias);
  }
  uint8_t GetActiveBits() const override
  {
    return Control::kValue.width;
  }

 private:
  const sjsu::Pin & dac_pin_;
};
}  // namespace lpc40xx
}  // namespace sjsu

#pragma once

#include "platforms/targets/lpc40xx/LPC40xx.h"
#include "peripherals/dac.hpp"
#include "peripherals/lpc40xx/pin.hpp"
#include "peripherals/lpc40xx/system_controller.hpp"
#include "utility/error_handling.hpp"
#include "utility/log.hpp"
#include "utility/math/limits.hpp"

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
    static constexpr bit::Mask kValue = bit::MaskFromRange(6, 15);

    /// Bias bit position in control register. See Bias enumeration for details
    /// about how this bit works.
    static constexpr bit::Mask kBias = bit::MaskFromRange(16);
  };

  /// The only DAC output pin on the lpc40xx.
  static inline sjsu::lpc40xx::Pin default_dac_pin = sjsu::lpc40xx::Pin(0, 26);

  /// Voltage reference for the lpc40xx voltage.
  static constexpr units::voltage::microvolt_t kVref = 3.3_V;

  /// Maximum numeric value of the ADC value register.
  static constexpr uint32_t kBitWidth = Control::kValue.width;

  /// Pointer to the LPC DAC peripheral in memory
  inline static LPC_DAC_TypeDef * dac_register = LPC_DAC;

  /// Construct lpc40xx Dac object.
  ///
  /// @param pin - defaults to the only dac pin on the board. The only reason to
  ///        use this parameter would be for unit testing. Otherwise, it should
  ///        not be changed from its default.
  explicit Dac(sjsu::Pin & pin = default_dac_pin) : dac_pin_(pin) {}

  /// The DAC is always connected to power, Initialize does nothing.
  void ModuleInitialize() override
  {
    static constexpr uint8_t kDacMode = 0b010;

    if constexpr (build::IsPlatform(build::Platform::lpc40xx))
    {
      // Temporarily convert dac_pin to a lpc40xx::Pin so we can use the
      // EnableDacs() method featured in the LPC40xx pin object.
      // The program is ill-formed if the pin's implementation was not a
      // lpc40xx pin.
      const sjsu::lpc40xx::Pin & lpc40xx_dac_pin =
          reinterpret_cast<const sjsu::lpc40xx::Pin &>(dac_pin_);
      lpc40xx_dac_pin.EnableDac();
    }

    constexpr PinSettings_t kDacPinSettings = {
      .function  = kDacMode,
      .resistor  = PinSettings_t::Resistor::kNone,
      .open_drain = false,
      .as_analog = true,
    };

    dac_pin_.settings = kDacPinSettings;
    dac_pin_.Initialize();

    // Disable interrupt and DMA
    dac_register->CTRL = 0;

    // Set Update Rate to 1MHz
    SetBias(Bias::kHigh);
  }

  void Write(uint32_t dac_output) override
  {
    // The DAC output is a 10 bit input and thus it is necessary to
    // ensure dac_output is less than 1024 (largest 10-bit number)
    dac_output = std::clamp(dac_output,
                            BitLimits<kBitWidth, uint32_t>::Min(),
                            BitLimits<kBitWidth, uint32_t>::Max());

    bit::Register(&dac_register->CR).Insert(dac_output, Control::kValue).Save();
  }

  void SetVoltage(units::voltage::microvolt_t voltage) override
  {
    auto value = (voltage * BitLimits<kBitWidth, uint32_t>::Max()) / kVref;
    return Write(value.to<uint32_t>());
  }

  /// Sets the Bias for the Dac, which determines the settling time, max
  /// current, and the allowed maximum update rate
  void SetBias(Bias bias_level) const
  {
    bool bias        = static_cast<bool>(bias_level);
    dac_register->CR = bit::Insert(dac_register->CR, bias, Control::kBias);
  }

  uint8_t GetActiveBits() override
  {
    return Control::kValue.width;
  }

 private:
  sjsu::Pin & dac_pin_;
};

template <int port>
inline Dac & GetDac()
{
  static_assert(port == 0, "LPC40xx only supports DAC0!");
  static Dac dac;
  return dac;
};
}  // namespace lpc40xx
}  // namespace sjsu

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
class Dac final : public sjsu::Dac, protected sjsu::lpc40xx::SystemController
{
 public:
  enum Bit : uint8_t
  {
    kBiasReg   = 16,
    kDacOutReg = 6,
  };
  enum class Bias : bool
  {
    kHigh = 0,
    kLow  = 1
  };

  union [[gnu::packed]] ControlRegister {
    uint32_t data;
    struct
    {
      uint8_t reserved0 : 6;
      uint16_t value : 10;
      bool bias : 1;
    } bits;
  };

  static constexpr sjsu::lpc40xx::Pin kDacPin = Pin::CreatePin<0, 26>();
  static constexpr float kVref = 3.3f;

  inline static LPC_DAC_TypeDef * dac_register = LPC_DAC;

  explicit constexpr Dac(const sjsu::Pin & pin = kDacPin) : dac_pin_(pin) {}
  /// Initialize DAC hardware and enable DAC Pin.
  /// Initial Bias level set to 0.
  Status Initialize() const override
  {
    static constexpr uint8_t kDacMode = 0b010;
    dac_pin_.SetPinFunction(kDacMode);
    // Temporally convert
    reinterpret_cast<const Pin *>(&dac_pin_)->EnableDac();
    dac_pin_.SetAsAnalogMode();
    dac_pin_.SetMode(Pin::Mode::kInactive);
    // Disable interrupt and DMA
    dac_register->CTRL = 0;
    // Set Update Rate to 1MHz
    SetBias(Bias::kHigh);

    return Status::kSuccess;
  }
  /// Set the digital-to-analog converter directly
  void Write(uint32_t dac_output) const override
  {
    // The DAC output is a 10 bit input and thus it is necessary to
    // ensure dac_output is less than 1024 (largest 10-bit number)
    SJ2_ASSERT_FATAL(dac_output < 1023,
                     "DAC output set above 1023. Must be between 0-1023.");
    GetControlRegister()->bits.value = dac_output & 0b11'1111'1111;
  }
  /// Takes an input voltage and converts the float value and calculates
  /// the conversion necessary and then typecasts it to an integer.
  /// If the voltage value is greater than 3.3 it will fail and end.
  void SetVoltage(float voltage) const override
  {
    float value         = (voltage * 1024.0f) / kVref;
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
    bool bias                       = static_cast<bool>(bias_level);
    GetControlRegister()->bits.bias = bias;
  }

 private:
  volatile ControlRegister * GetControlRegister() const
  {
    return reinterpret_cast<volatile ControlRegister *>(&dac_register->CR);
  }
  const sjsu::Pin & dac_pin_;
};
}  // namespace lpc40xx
}  // namespace sjsu

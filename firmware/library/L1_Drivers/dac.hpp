// Dac is a driver for the Digital-To-Analog converter for the LPC40xx chip
#pragma once
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/pin.hpp"

class DacInterface
{
 public:
  virtual void Initialize(void)              = 0;
  virtual bool WriteDac(uint16_t dac_output) = 0;
  virtual bool SetVoltage(float voltage)     = 0;
};
class Dac : public DacInterface
{
 public:
  static LPC_DAC_TypeDef * dac_register;
  static LPC_SC_TypeDef * sc_ptr;
  static LPC_IOCON_TypeDef * iocon_register;
  static constexpr float kVref        = 3.3f;
  static constexpr uint8_t kDacMode   = 0b010;
  static constexpr uint16_t kClearDac = 0b1111111111;
  static constexpr uint8_t kClockDiv  = 0b0010;
  enum Bit : uint8_t
  {
    kBiasReg   = 16,
    kDacOutReg = 6,
  };
  enum class Bias : uint8_t
  {
    kHigh = 0,
    kLow  = 1
  };
  constexpr Dac() : dac_(&dac_pin_), dac_pin_(0, 26) {}
  // Supply your own pin for DAC output.
  explicit constexpr Dac(PinInterface * dac_pin)
      : dac_(dac_pin), dac_pin_(Pin::CreateInactivePin())  // P0.26
  {
  }
  // Initialize DAC hardware and enable DAC Pin.
  // Initial Bias level set to 0.
  void Initialize(void) override
  {
    dac_->SetPinFunction(kDacMode);
    dac_->EnableDac(true);
    dac_->SetAsAnalogMode(true);
    dac_->SetMode(PinInterface::Mode::kInactive);
    // Set Update Rate to 1MHz
    SetBias(Bias::kHigh);
  }
  // Set the digital to analog converter
  bool WriteDac(uint16_t dac_output) override
  {
    // The DAC output is a 10 bit input and thus it is necessary to
    // ensure dac_output is less than 1024 (largest 10-bit number)
    SJ2_ASSERT_FATAL(dac_output < 1023,
                     "DAC output set above 1023. Must be between 0-1023.");
    dac_register->CR &= ~(kClearDac << kDacOutReg);
    dac_register->CR |= (dac_output << kDacOutReg);
    return true;
  }
  // Takes an input voltage and converts the float value and calculates
  // the conversion necessary and then typecasts it to an integer.
  // If the voltage value is greater than 3.3 it will fail and end.
  bool SetVoltage(float voltage) override
  {
    // value            = (dac_out * 1024)/VrefP
    float value         = (voltage * 1024.0f) / kVref;
    uint16_t conversion = static_cast<uint16_t>(value);
    SJ2_ASSERT_FATAL(
        voltage < kVref,
        "DAC output was set above 3.3V. Must be between 0V and 3.3V.");
    WriteDac(conversion);
    return true;
  }
  // Sets the Bias for the Dac, which determines the settling time, max current,
  // and the allowed maximum update rate
  void SetBias(Bias bias_level)
  {
    uint8_t bias = static_cast<uint8_t>(bias_level);
    dac_register->CR =
        (dac_register->CR & ~(1 << kBiasReg)) | (bias << kBiasReg);
  }

 private:
  PinInterface * dac_;
  Pin dac_pin_;
};

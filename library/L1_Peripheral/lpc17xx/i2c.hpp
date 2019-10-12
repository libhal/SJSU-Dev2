#pragma once

#include "L1_Peripheral/lpc17xx/pin.hpp"
#include "L1_Peripheral/lpc17xx/system_controller.hpp"
#include "L1_Peripheral/lpc40xx/i2c.hpp"

namespace sjsu
{
namespace lpc17xx
{
// Bring in and using the LPC40xx driver as it is compatible with the lpc17xx
// peripheral.
using ::sjsu::lpc40xx::I2c;

/// Structure used as a namespace for predefined I2C Bus definitions.
struct I2cBus  // NOLINT
{
 private:
  inline static const Pin kI2c0SdaPin = Pin::CreatePin<0, 27>();
  inline static const Pin kI2c0SclPin = Pin::CreatePin<0, 28>();
  inline static const Pin kI2c1SdaPin = Pin::CreatePin<0, 0>();
  inline static const Pin kI2c1SclPin = Pin::CreatePin<0, 1>();
  inline static const Pin kI2c2SdaPin = Pin::CreatePin<0, 10>();
  inline static const Pin kI2c2SclPin = Pin::CreatePin<0, 11>();

  inline static I2c::Transaction_t transaction_i2c0;
  inline static const lpc40xx::I2c::PartialBus_t kI2c0Partial = {
    .registers = reinterpret_cast<lpc40xx::LPC_I2C_TypeDef *>(LPC_I2C0),
    .peripheral_power_id = SystemController::Peripherals::kI2c0,
    .irq_number          = I2C0_IRQn,
    .transaction         = transaction_i2c0,
    .sda_pin             = kI2c0SdaPin,
    .scl_pin             = kI2c0SclPin,
    .pin_function_id     = 0b01,
  };

  inline static I2c::Transaction_t transaction_i2c1;
  inline static const lpc40xx::I2c::PartialBus_t kI2c1Partial = {
    .registers = reinterpret_cast<lpc40xx::LPC_I2C_TypeDef *>(LPC_I2C1),
    .peripheral_power_id = SystemController::Peripherals::kI2c1,
    .irq_number          = I2C1_IRQn,
    .transaction         = transaction_i2c1,
    .sda_pin             = kI2c1SdaPin,
    .scl_pin             = kI2c1SclPin,
    .pin_function_id     = 0b11,
  };

  inline static I2c::Transaction_t transaction_i2c2;
  inline static const lpc40xx::I2c::PartialBus_t kI2c2Partial = {
    .registers = reinterpret_cast<lpc40xx::LPC_I2C_TypeDef *>(LPC_I2C2),
    .peripheral_power_id = SystemController::Peripherals::kI2c2,
    .irq_number          = I2C2_IRQn,
    .transaction         = transaction_i2c2,
    .sda_pin             = kI2c2SdaPin,
    .scl_pin             = kI2c2SclPin,
    .pin_function_id     = 0b10,
  };

 public:
  /// Definition for I2C bus 0 for LPC17xx.
  /// NOTE: I2C0 is not available for the 80-pin package.
  inline static const lpc40xx::I2c::Bus_t kI2c0 = {
    .bus     = kI2c0Partial,
    .handler = lpc40xx::I2c::I2cHandler<kI2c0Partial>,
  };
  /// Definition for I2C bus 1 for LPC17xx.
  inline static const lpc40xx::I2c::Bus_t kI2c1 = {
    .bus     = kI2c1Partial,
    .handler = lpc40xx::I2c::I2cHandler<kI2c1Partial>,
  };
  /// Definition for I2C bus 2 for LPC17xx.
  inline static const lpc40xx::I2c::Bus_t kI2c2 = {
    .bus     = kI2c2Partial,
    .handler = lpc40xx::I2c::I2cHandler<kI2c2Partial>,
  };
};
}  // namespace lpc17xx
}  // namespace sjsu

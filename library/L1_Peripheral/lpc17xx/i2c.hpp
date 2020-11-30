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
  inline static lpc17xx::Pin i2c0_sda_pin = lpc17xx::Pin(0, 27);
  inline static lpc17xx::Pin i2c0_scl_pin = lpc17xx::Pin(0, 28);

  inline static lpc17xx::Pin i2c1_sda_pin = lpc17xx::Pin(0, 0);
  inline static lpc17xx::Pin i2c1_scl_pin = lpc17xx::Pin(0, 1);

  inline static lpc17xx::Pin i2c2_sda_pin = lpc17xx::Pin(0, 10);
  inline static lpc17xx::Pin i2c2_scl_pin = lpc17xx::Pin(0, 11);

  inline static I2c::Transaction_t transaction_i2c0;
  inline static I2c::Transaction_t transaction_i2c1;
  inline static I2c::Transaction_t transaction_i2c2;

 public:
  // Definition for I2C bus 0 for LPC17xx.
  /// NOTE: I2C0 is not available for the 80-pin package.
  inline static const lpc40xx::I2c::Bus_t kI2c0 = {
    .registers    = reinterpret_cast<lpc40xx::LPC_I2C_TypeDef *>(LPC_I2C0),
    .id           = SystemController::Peripherals::kI2c0,
    .irq_number   = I2C0_IRQn,
    .transaction  = transaction_i2c0,
    .sda_pin      = i2c0_sda_pin,
    .scl_pin      = i2c0_scl_pin,
    .pin_function = 0b01,
  };

  /// Definition for I2C bus 1 for LPC17xx.
  inline static const lpc40xx::I2c::Bus_t kI2c1 = {
    .registers    = reinterpret_cast<lpc40xx::LPC_I2C_TypeDef *>(LPC_I2C1),
    .id           = SystemController::Peripherals::kI2c1,
    .irq_number   = I2C1_IRQn,
    .transaction  = transaction_i2c1,
    .sda_pin      = i2c1_sda_pin,
    .scl_pin      = i2c1_scl_pin,
    .pin_function = 0b11,
  };

  /// Definition for I2C bus 2 for LPC17xx.
  inline static const lpc40xx::I2c::Bus_t kI2c2 = {
    .registers    = reinterpret_cast<lpc40xx::LPC_I2C_TypeDef *>(LPC_I2C2),
    .id           = SystemController::Peripherals::kI2c2,
    .irq_number   = I2C2_IRQn,
    .transaction  = transaction_i2c2,
    .sda_pin      = i2c2_sda_pin,
    .scl_pin      = i2c2_scl_pin,
    .pin_function = 0b10,
  };
};
}  // namespace lpc17xx
}  // namespace sjsu

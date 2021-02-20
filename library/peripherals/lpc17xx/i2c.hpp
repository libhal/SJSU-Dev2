#pragma once

#include "peripherals/lpc17xx/pin.hpp"
#include "peripherals/lpc17xx/system_controller.hpp"
#include "peripherals/lpc40xx/i2c.hpp"

namespace sjsu
{
namespace lpc17xx
{
// Bring in and using the LPC40xx driver as it is compatible with the lpc17xx
// peripheral.
using ::sjsu::lpc40xx::I2c;

template <int port>
inline I2c & GetI2c()
{
  // UM10562: Chapter 7: LPC408x/407x I/O configuration page 13
  if constexpr (port == 0)
  {
    static auto & i2c0_sda_pin = lpc17xx::GetPin<0, 27>();
    static auto & i2c0_scl_pin = lpc17xx::GetPin<0, 28>();

    static I2c::Transaction_t transaction_i2c0;

    static const lpc40xx::I2c::Port_t kI2c0 = {
      .registers    = reinterpret_cast<lpc40xx::LPC_I2C_TypeDef *>(LPC_I2C0),
      .id           = SystemController::Peripherals::kI2c0,
      .irq_number   = I2C0_IRQn,
      .transaction  = transaction_i2c0,
      .sda_pin      = i2c0_sda_pin,
      .scl_pin      = i2c0_scl_pin,
      .pin_function = 0b01,
    };

    static I2c i2c0(kI2c0);
    return i2c0;
  }
  else if constexpr (port == 1)
  {
    static auto & i2c1_sda_pin = lpc17xx::GetPin<0, 0>();
    static auto & i2c1_scl_pin = lpc17xx::GetPin<0, 1>();

    static I2c::Transaction_t transaction_i2c1;

    static const lpc40xx::I2c::Port_t kI2c1 = {
      .registers    = reinterpret_cast<lpc40xx::LPC_I2C_TypeDef *>(LPC_I2C1),
      .id           = SystemController::Peripherals::kI2c1,
      .irq_number   = I2C1_IRQn,
      .transaction  = transaction_i2c1,
      .sda_pin      = i2c1_sda_pin,
      .scl_pin      = i2c1_scl_pin,
      .pin_function = 0b11,
    };

    static I2c i2c1(kI2c1);
    return i2c1;
  }
  else if constexpr (port == 2)
  {
    static auto & i2c2_sda_pin = lpc17xx::GetPin<0, 10>();
    static auto & i2c2_scl_pin = lpc17xx::GetPin<0, 11>();

    static I2c::Transaction_t transaction_i2c2;

    static const lpc40xx::I2c::Port_t kI2c2 = {
      .registers    = reinterpret_cast<lpc40xx::LPC_I2C_TypeDef *>(LPC_I2C2),
      .id           = SystemController::Peripherals::kI2c2,
      .irq_number   = I2C2_IRQn,
      .transaction  = transaction_i2c2,
      .sda_pin      = i2c2_sda_pin,
      .scl_pin      = i2c2_scl_pin,
      .pin_function = 0b10,
    };

    static I2c i2c2(kI2c2);
    return i2c2;
  }
  else
  {
    static_assert(InvalidOption<port>,
                  "Support UART ports for LPC40xx are I2C0, I2C1, and I2C2.");
    return GetI2c<0>();
  }
}
}  // namespace lpc17xx
}  // namespace sjsu

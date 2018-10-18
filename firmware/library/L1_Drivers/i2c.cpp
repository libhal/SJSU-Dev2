#include <cstdint>
#include "L0_LowLevel/LPC40xx.h"
#include "L1_Drivers/i2c.hpp"
#include "L2_Utilities/enum.hpp"

constexpr uint8_t kI2c0 = util::Value(I2c::Port::kI2c0);
constexpr uint8_t kI2c1 = util::Value(I2c::Port::kI2c1);
constexpr uint8_t kI2c2 = util::Value(I2c::Port::kI2c2);

const uint8_t I2c::kPconp[I2c::kNumberOfPorts] = {
  [kI2c0] = 7, [kI2c1] = 19, [kI2c2] = 26
};

const IRQn_Type I2c::kIrq[I2c::kNumberOfPorts] = {
  [kI2c0] = I2C0_IRQn, [kI2c1] = I2C1_IRQn, [kI2c2] = I2C2_IRQn
};

LPC_I2C_TypeDef * I2c::i2c[I2c::kNumberOfPorts] = {
  [kI2c0] = LPC_I2C0, [kI2c1] = LPC_I2C1, [kI2c2] = LPC_I2C2
};

IsrPointer I2c::handlers[I2c::kNumberOfPorts] = {
  [kI2c0] = I2c::I2cHandler<I2c::Port::kI2c0>,
  [kI2c1] = I2c::I2cHandler<I2c::Port::kI2c1>,
  [kI2c2] = I2c::I2cHandler<I2c::Port::kI2c2>
};

I2c::Transaction_t I2c::transaction[kNumberOfPorts];

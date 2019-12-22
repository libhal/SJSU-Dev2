#pragma once

#include "L1_Peripheral/lpc17xx/adc.hpp"
#include "L1_Peripheral/lpc17xx/gpio.hpp"
#include "L1_Peripheral/lpc17xx/i2c.hpp"
#include "L1_Peripheral/lpc17xx/spi.hpp"
#include "L2_HAL/memory/sd.hpp"
#include "L2_HAL/sensors/environment/light/temt6000x01.hpp"
#include "L2_HAL/sensors/environment/temperature/tmp102.hpp"
#include "L2_HAL/sensors/movement/accelerometer/mma8452q.hpp"

struct sjtwo  // NOLINT
{
  inline static sjsu::lpc17xx::Spi spi0 =
      sjsu::lpc17xx::Spi(sjsu::lpc17xx::Spi::Bus::kSpi0);
  inline static sjsu::lpc17xx::Spi spi1 =
      sjsu::lpc17xx::Spi(sjsu::lpc17xx::Spi::Bus::kSpi1);
  inline static sjsu::lpc17xx::Spi spi2 =
      sjsu::lpc17xx::Spi(sjsu::lpc17xx::Spi::Bus::kSpi2);

  inline static sjsu::lpc17xx::I2c i2c0 =
      sjsu::lpc17xx::I2c(sjsu::lpc17xx::I2c::Bus::kI2c0);
  inline static sjsu::lpc17xx::I2c i2c1 =
      sjsu::lpc17xx::I2c(sjsu::lpc17xx::I2c::Bus::kI2c1);
  inline static sjsu::lpc17xx::I2c i2c2 =
      sjsu::lpc17xx::I2c(sjsu::lpc17xx::I2c::Bus::kI2c2);

  inline static sjsu::lpc40xx::Gpio led0 = sjsu::lpc40xx::Gpio(1, 0);
  inline static sjsu::lpc40xx::Gpio led1 = sjsu::lpc40xx::Gpio(1, 1);
  inline static sjsu::lpc40xx::Gpio led2 = sjsu::lpc40xx::Gpio(1, 4);
  inline static sjsu::lpc40xx::Gpio led3 = sjsu::lpc40xx::Gpio(1, 8);

  inline static sjsu::lpc17xx::Adc adc2 =
      sjsu::lpc17xx::Adc(sjsu::lpc17xx::AdcChannel::kChannel2);

  [[gnu::always_inline]] inline static sjsu::Mma8452q & Accelerometer()
  {
    static sjsu::Mma8452q accelerometer(i2c2);
    return accelerometer;
  }

  [[gnu::always_inline]] inline static sjsu::Sd & SdCard()
  {
    static sjsu::lpc17xx::Gpio sd_cs = sjsu::lpc17xx::Gpio(1, 8);
    static sjsu::Sd sd(spi2, sd_cs);
    return sd;
  }

  [[gnu::always_inline]] inline static sjsu::Tmp102 & Temperature()
  {
    static sjsu::Tmp102 tmp102(i2c2);
    return tmp102;
  }

  [[gnu::always_inline]] inline static sjsu::Temt6000x01 & LightSensor()
  {
    // The LPC176x/5x ADC has a reference voltage of 3.3V.
    constexpr units::voltage::volt_t kAdcReferenceVoltage = 3.3_V;
    // A 10kOhm pull-down resistor is used on the SJOne board.
    constexpr units::impedance::ohm_t kPullDownResistance = 10_kOhm;
    static sjsu::Temt6000x01 temt6000(
        adc2, kAdcReferenceVoltage, kPullDownResistance);
    return temt6000;
  }
};

#pragma once

#include "L1_Peripheral/lpc17xx/gpio.hpp"
#include "L1_Peripheral/lpc17xx/i2c.hpp"
#include "L1_Peripheral/lpc17xx/spi.hpp"
#include "L2_HAL/displays/led/onboard_led.hpp"
#include "L2_HAL/memory/sd.hpp"
#include "L2_HAL/sensors/environment/temperature/tmp102.hpp"
#include "L2_HAL/sensors/movement/accelerometer/mma8452q.hpp"

struct sjtwo // NOLINT
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

  [[gnu::always_inline]] inline static sjsu::OnBoardLed & Leds()
  {
    static sjsu::OnBoardLed leds;
    return leds;
  }

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
};

#pragma once

#include "L1_Peripheral/lpc40xx/gpio.hpp"
#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L1_Peripheral/lpc40xx/spi.hpp"

#include "L2_HAL/displays/led/onboard_led.hpp"
#include "L2_HAL/displays/oled/ssd1306.hpp"
#include "L2_HAL/memory/sd.hpp"
#include "L2_HAL/sensors/environment/temperature/si7060.hpp"
#include "L2_HAL/sensors/movement/accelerometer/mma8452q.hpp"
#include "L2_HAL/sensors/optical/apds9960.hpp"

#include "L3_Application/graphics.hpp"

struct sjtwo // NOLINT
{
  inline static sjsu::lpc40xx::Spi spi0 =
      sjsu::lpc40xx::Spi(sjsu::lpc40xx::Spi::Bus::kSpi0);
  inline static sjsu::lpc40xx::Spi spi1 =
      sjsu::lpc40xx::Spi(sjsu::lpc40xx::Spi::Bus::kSpi1);
  inline static sjsu::lpc40xx::Spi spi2 =
      sjsu::lpc40xx::Spi(sjsu::lpc40xx::Spi::Bus::kSpi2);

  inline static sjsu::lpc40xx::I2c i2c0 =
      sjsu::lpc40xx::I2c(sjsu::lpc40xx::I2c::Bus::kI2c0);
  inline static sjsu::lpc40xx::I2c i2c1 =
      sjsu::lpc40xx::I2c(sjsu::lpc40xx::I2c::Bus::kI2c1);
  inline static sjsu::lpc40xx::I2c i2c2 =
      sjsu::lpc40xx::I2c(sjsu::lpc40xx::I2c::Bus::kI2c2);

  [[gnu::always_inline]] inline static sjsu::Graphics & Oled()
  {
    static sjsu::lpc40xx::Gpio oled_cs = sjsu::lpc40xx::Gpio(1, 22);
    static sjsu::lpc40xx::Gpio oled_dc = sjsu::lpc40xx::Gpio(1, 25);
    static sjsu::Ssd1306 oled_display(spi1, oled_cs, oled_dc);
    static sjsu::Graphics oled(oled_display);
    return oled;
  }

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
    static sjsu::lpc40xx::Gpio sd_cs = sjsu::lpc40xx::Gpio(1, 8);
    static sjsu::Sd sd(spi2, sd_cs);
    return sd;
  }

  [[gnu::always_inline]] inline static sjsu::Apds9960 & Gesture()
  {
    static sjsu::Apds9960 apds9960(i2c2);
    return apds9960;
  }

  [[gnu::always_inline]] inline static sjsu::Si7060 & Temperature()
  {
    static sjsu::Si7060 si7060(i2c2);
    return si7060;
  }
};

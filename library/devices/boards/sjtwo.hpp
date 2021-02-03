#pragma once

#include "peripherals/inactive.hpp"
#include "peripherals/lpc40xx/gpio.hpp"
#include "peripherals/lpc40xx/i2c.hpp"
#include "peripherals/lpc40xx/spi.hpp"
#include "devices/displays/oled/ssd1306.hpp"
#include "devices/memory/sd.hpp"
#include "devices/sensors/environment/temperature/si7060.hpp"
#include "devices/sensors/movement/accelerometer/mma8452q.hpp"
// #include "devices/sensors/optical/apds9960.hpp"

#include "systems/graphics/graphics.hpp"

/// Namespace for SJTwo board peripheral and device definitions
struct sjtwo  // NOLINT
{
  /// Predefined SJTwo spi0 peripheral
  inline static sjsu::lpc40xx::Spi & spi0 = sjsu::lpc40xx::GetSpi<0>();
  /// Predefined SJTwo spi1 peripheral
  inline static sjsu::lpc40xx::Spi & spi1 = sjsu::lpc40xx::GetSpi<1>();
  /// Predefined SJTwo spi2 peripheral
  inline static sjsu::lpc40xx::Spi & spi2 = sjsu::lpc40xx::GetSpi<2>();

  /// Predefined SJTwo i2c0 peripheral
  inline static sjsu::lpc40xx::I2c & i2c0 = sjsu::lpc40xx::GetI2c<0>();
  /// Predefined SJTwo i2c1 peripheral
  inline static sjsu::lpc40xx::I2c & i2c1 = sjsu::lpc40xx::GetI2c<1>();
  /// Predefined SJTwo i2c2 peripheral
  inline static sjsu::lpc40xx::I2c & i2c2 = sjsu::lpc40xx::GetI2c<2>();

  /// Predefined SJTwo led0 peripheral
  inline static sjsu::lpc40xx::Gpio & led0 = sjsu::lpc40xx::GetGpio<2, 3>();
  /// Predefined SJTwo led1 peripheral
  inline static sjsu::lpc40xx::Gpio & led1 = sjsu::lpc40xx::GetGpio<1, 26>();
  /// Predefined SJTwo led2 peripheral
  inline static sjsu::lpc40xx::Gpio & led2 = sjsu::lpc40xx::GetGpio<1, 24>();
  /// Predefined SJTwo led3 peripheral
  inline static sjsu::lpc40xx::Gpio & led3 = sjsu::lpc40xx::GetGpio<1, 18>();

  /// Predefined SJTwo button0 peripheral
  inline static sjsu::lpc40xx::Gpio & button0 = sjsu::lpc40xx::GetGpio<1, 19>();
  /// Predefined SJTwo button1 peripheral
  inline static sjsu::lpc40xx::Gpio & button1 = sjsu::lpc40xx::GetGpio<1, 15>();
  /// Predefined SJTwo button2 peripheral
  inline static sjsu::lpc40xx::Gpio & button2 = sjsu::lpc40xx::GetGpio<0, 30>();
  /// Predefined SJTwo button3 peripheral
  inline static sjsu::lpc40xx::Gpio & button3 = sjsu::lpc40xx::GetGpio<0, 29>();

  /// @returns fully constructed onboard Oled device driver
  inline static sjsu::Graphics & Oled()
  {
    static sjsu::lpc40xx::Gpio oled_cs = sjsu::lpc40xx::Gpio(1, 22);
    static sjsu::lpc40xx::Gpio oled_dc = sjsu::lpc40xx::Gpio(1, 25);
    static sjsu::Ssd1306 oled_display(
        spi1, oled_cs, oled_dc, sjsu::GetInactive<sjsu::Gpio>());
    static sjsu::Graphics oled(oled_display);
    return oled;
  }

  /// @returns fully constructed onboard Accelerometer device driver
  inline static sjsu::Mma8452q & Accelerometer()
  {
    static sjsu::Mma8452q accelerometer(i2c2);
    return accelerometer;
  }

  /// @returns fully constructed onboard Sd Card device driver
  inline static sjsu::Sd & SdCard()
  {
    static sjsu::lpc40xx::Gpio sd_cs = sjsu::lpc40xx::Gpio(1, 8);
    static sjsu::Sd sd(spi2, sd_cs, sjsu::GetInactive<sjsu::Gpio>());
    return sd;
  }

  // /// @returns fully constructed onboard Gesture device driver
  // inline static sjsu::Apds9960 & Gesture()
  // {
  //   static sjsu::Apds9960 apds9960(i2c2);
  //   return apds9960;
  // }

  /// @returns fully constructed onboard Temperature device driver
  inline static sjsu::Si7060 & Temperature()
  {
    static sjsu::Si7060 si7060(i2c2);
    return si7060;
  }
};

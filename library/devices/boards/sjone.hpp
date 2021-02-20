#pragma once

#include "peripherals/lpc17xx/adc.hpp"
#include "peripherals/lpc17xx/gpio.hpp"
#include "peripherals/lpc17xx/i2c.hpp"
#include "peripherals/lpc17xx/spi.hpp"
#include "devices/memory/sd.hpp"
#include "devices/sensors/environment/light/temt6000x01.hpp"
#include "devices/sensors/environment/temperature/tmp102.hpp"
#include "devices/sensors/movement/accelerometer/mma8452q.hpp"

/// Namespace for SJOne board peripheral and device definitions
struct sjone  // NOLINT
{
  /// Predefined SJOne spi0 peripheral
  inline static sjsu::lpc17xx::Spi & spi0 = sjsu::lpc17xx::GetSpi<0>();
  /// Predefined SJOne spi1 peripheral
  inline static sjsu::lpc17xx::Spi & spi1 = sjsu::lpc17xx::GetSpi<1>();

  /// Predefined SJOne i2c0 peripheral
  inline static sjsu::lpc17xx::I2c & i2c0 = sjsu::lpc17xx::GetI2c<0>();
  /// Predefined SJOne i2c1 peripheral
  inline static sjsu::lpc17xx::I2c & i2c1 = sjsu::lpc17xx::GetI2c<1>();
  /// Predefined SJOne i2c2 peripheral
  inline static sjsu::lpc17xx::I2c & i2c2 = sjsu::lpc17xx::GetI2c<2>();

  /// Predefined SJOne led0 peripheral
  inline static sjsu::lpc40xx::Gpio & led0 = sjsu::lpc40xx::GetGpio<1, 0>();
  /// Predefined SJOne led1 peripheral
  inline static sjsu::lpc40xx::Gpio & led1 = sjsu::lpc40xx::GetGpio<1, 1>();
  /// Predefined SJOne led2 peripheral
  inline static sjsu::lpc40xx::Gpio & led2 = sjsu::lpc40xx::GetGpio<1, 4>();
  /// Predefined SJOne led3 peripheral
  inline static sjsu::lpc40xx::Gpio & led3 = sjsu::lpc40xx::GetGpio<1, 8>();

  /// Predefined SJOne adc2 peripheral
  inline static sjsu::lpc17xx::Adc & adc2 = sjsu::lpc17xx::GetAdc<2>();

  /// @returns fully constructed onboard Mma8452q device driver
  inline static sjsu::Mma8452q & Accelerometer()
  {
    static sjsu::Mma8452q accelerometer(i2c2);
    return accelerometer;
  }

  /// @returns fully constructed onboard Sd device driver
  inline static sjsu::Sd & SdCard()
  {
    static sjsu::lpc17xx::Gpio sd_cs = sjsu::lpc17xx::Gpio(2, 6);
    static sjsu::Sd sd(spi1, sd_cs, sjsu::GetInactive<sjsu::Gpio>());
    return sd;
  }

  /// @returns fully constructed onboard Tmp102 device driver
  inline static sjsu::Tmp102 & Temperature()
  {
    static sjsu::Tmp102 tmp102(i2c2);
    return tmp102;
  }

  /// @returns fully constructed onboard Temt6000x01 device driver
  inline static sjsu::Temt6000x01 & LightSensor()
  {
    // A 10kOhm pull-down resistor is used on the SJOne board.
    constexpr units::impedance::ohm_t kPullDownResistance = 10_kOhm;
    static sjsu::Temt6000x01 temt6000(adc2, kPullDownResistance);
    return temt6000;
  }
};

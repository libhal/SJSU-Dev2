/* Interface used to obtain the temperature of the system.*/
#pragma once

#ifndef TEMPERATURE_SENSOR_HPP_

#define TEMPERATURE_SENSOR_HPP_



#include "i2c_base.hpp"

class TemperatureSensor : private i2c_base

{
 public:
  virtual bool init();
  // Initialize Drivers

  virtual unsigned char
  getTemperatureByte();
  // Read from register 0x00 and return byte.

  virtual float getCelsius();
  // Converting bits from register 0x00 to float.

  virtual float getFahrenheit();
  // Converting Celsius to Fahrenheit.
};

#endif /*TEMPERATURE_SENSOR_HPP_*/

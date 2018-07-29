/* Interface used to obtain the temperature of the system.*/
#pragma once

#ifndef TEMPERATURE_SENSOR_HPP_

#define TEMPERATURE_SENSOR_HPP_



#include "i2c2_device.hpp"

class TemperatureSensor : private i2c2_device

{
 public:
  virtual bool Init();
  // Initialize Drivers

  virtual unsigned char
  GetTemperatureByte();
  // Read from register 0x00 and return byte.

  virtual float GetCelsius();
  // Converting bits from register 0x00 to float.

  virtual float GetFahrenheit();
  // Converting Celsius to Fahrenheit.
};

#endif /*TEMPERATURE_SENSOR_HPP_*/

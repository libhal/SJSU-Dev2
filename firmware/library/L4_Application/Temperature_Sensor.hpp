/* Interface used to obtain the temperature of the system.*/
#pragma once

class TemperatureInterface

{
 public:
  virtual bool Init() = 0;
  // Initialize Drivers

  virtual unsigned char
  GetTemperatureByte() = 0;
  // Read from register 0x00 and return byte.

  virtual float GetCelsius() = 0;
  // Converting bits from register 0x00 to float.

  virtual float GetFahrenheit() = 0;
  // Converting Celsius to Fahrenheit.
};


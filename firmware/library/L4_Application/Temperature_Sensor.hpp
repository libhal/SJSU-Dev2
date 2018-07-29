// Interface used to sense the temperature of the temperature sensor.
#pragma once
#include <cstdint>
class TemperatureInterface
{
 public:
  virtual bool Initialize() = 0;

  // Read from register 0x00 and return byte.
  virtual uint8_t GetTemperatureByte() = 0;

  // Converts bits from register 0x00 to float.
  virtual float GetCelsius() = 0;

  // Converts Celsius to Fahrenheit.
  virtual float GetFahrenheit() = 0;
};

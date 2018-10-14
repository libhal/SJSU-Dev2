// Interface used to sense the temperature of the temperature sensor.
#pragma once
#include <cstdint>
class TemperatureInterface
{
 public:
  virtual bool Initialize() = 0;
  virtual uint32_t GetTemperatureBytes() = 0;
  virtual float GetCelsius() = 0;
  virtual float GetFahrenheit() = 0;
};

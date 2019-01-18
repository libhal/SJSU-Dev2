#pragma once

#include "L1_Drivers/i2c.hpp"

class TemperatureInterface
{
 public:
  virtual bool Initialize()     = 0;
  virtual float GetCelsius()    = 0;
  virtual float GetFahrenheit() = 0;
};


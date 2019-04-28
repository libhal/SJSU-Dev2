#pragma once

namespace sjsu
{
class Temperature
{
 public:
  virtual bool Initialize()     = 0;
  virtual float GetCelsius()    = 0;
  virtual float GetFahrenheit() = 0;
};
}  // namespace sjsu

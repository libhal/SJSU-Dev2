#pragma once

#include <cstdint>

namespace sjsu
{
class Accelerometer
{
 public:
  virtual bool Initialize()                           = 0;
  virtual int16_t X() const                           = 0;
  virtual int16_t Y() const                           = 0;
  virtual int16_t Z() const                           = 0;
  virtual float Pitch() const                         = 0;
  virtual float Roll() const                          = 0;
  virtual int GetFullScaleRange() const               = 0;
  virtual void SetFullScaleRange(uint8_t range_value) = 0;
};
}  // namespace sjsu

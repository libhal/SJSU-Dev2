#pragma once

#include <cmath>
#include <cstdint>

class AccelerometerInterface
{
 public:
  virtual bool Init();
  virtual int16_t GetX();
  virtual int16_t GetY();
  virtual int16_t GetZ();
  virtual float GetPitch();
  virtual float GetRoll();
  virtual uint8_t GetFullScaleRange();
  virtual void SetFullScaleRange(uint8_t range_value);
};

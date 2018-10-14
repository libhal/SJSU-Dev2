#pragma once

#include <cstdint>

class Apds9960Interface
{
 public:
  virtual void Initialize()                                    = 0;
  virtual void EnableGesture()                                 = 0;
  virtual void DisableGesture()                                = 0;
  virtual void DecodeGestureData()                             = 0;
  virtual uint8_t GetGesture()                                 = 0;
  virtual uint8_t ReadMode()                                   = 0;
  virtual uint8_t GetGestureEnterThreshhold()                  = 0;
  virtual uint8_t GetGestureExitThreshhold()                   = 0;
  virtual bool SetGestureEnterThresh(uint8_t threshhold_value) = 0;
  virtual bool SetGestureExitThresh(uint8_t threshhold_value)  = 0;
  virtual bool CheckIfGestureOccured()                         = 0;
  virtual void ReadGestureFIFO()                               = 0;
};

#pragma once

class ButtonInterface
{
 public:
  virtual void Init()                                               = 0;
  virtual bool Read()                                               = 0;
  virtual bool Released()                                           = 0;
  virtual bool Pressed()                                            = 0;
  virtual void InvertButtonSignal(bool enable_invert_signal = true) = 0;
};

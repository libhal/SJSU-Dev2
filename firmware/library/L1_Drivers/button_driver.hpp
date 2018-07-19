#pragma once

class ButtonInterface {
 public:
         virtual void Init()                = 0;
         virtual bool GetButtonState()      = 0;
         virtual bool Released()            = 0;
         virtual bool Pressed()             = 0;
         virtual void DefaultButtonSignal() = 0;
         virtual void InvertButtonSignal()  = 0;
};
